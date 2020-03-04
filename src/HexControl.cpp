#include <string>
#include <optional>
#include <future>
#include <chrono>
#include <iostream>
#include <stdarg.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "Mask.h"
#include "MaskedImage.h"
#include "FrameResult.h"
#include "Processing.h"

#pragma region Functions
static std::optional<hex::FrameResult> processFrame(cv::InputArray src, std::vector<hex::Mask>& maskDefinitions);
static void display(cv::InputArray src, std::optional<hex::FrameResult> frame);
static void getColourMasks(cv::InputArray src, std::vector<hex::Mask> maskDefinitions, std::vector<hex::MaskedImage>& masks);
static void drawAllContours(cv::InputOutputArray& image, cv::InputArrayOfArrays contours, cv::InputArray hierarchy, int thickness = 1, int lineType = cv::LINE_8);
static bool isProminentRectangularContour(std::vector<cv::Point> contour);
static void cropAndStraigthen(cv::RotatedRect bounds, cv::InputArray input, cv::Mat& output);
#pragma endregion

#pragma region Helpers
static void createWindow(cv::String name, cv::InputArray& image);
static void createWindow(cv::String name, cv::InputArray& image, int width, int height);
static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0);

// Console Output Helpers
unsigned ln = 1;
auto colour(int n, const std::string& s) { return "\33[38;5;" + std::to_string(n) + 'm' + s + "\33[m"; }
auto line(int l) { int m = l - ln; ln = l; return "\r" + (m < 0 ? "\33[" + std::to_string(-m) + 'A' : std::string(m, '\n')); }
#pragma endregion

constexpr double imageProcessingScale = 0.6;
constexpr double contourThicknessScale = 1;
constexpr int displayHeight = 768;

#ifdef TRAINING
    std::string character =   "Z";
    std::string orientation = "3"; // O, 1, 2, 3 - Clockwise rotation (paper has to be rotated anti-clockwise from behind)
    constexpr int loopTiming = 4;

    int loop_timer = loopTiming;
    int trainingDataCounter = 0;
    #define WRITE_TRAINING_DATA() if (result.has_value()) cv::imwrite("training\\" + character + "\\" + orientation + "\\" + std::to_string(trainingDataCounter++) + ".bmp", result->result);
#else
    #define WRITE_TRAINING_DATA()
#endif

int main(int argc, char** argv)
{
	#pragma region Input
    // Create the video capture feed for interacting with the camera.
    cv::VideoCapture video(0);

    // Check if the camera has been opened successfully.
    if (!video.isOpened())
	{
        std::cout << "Error opening video stream or file" << std::endl;
		return -1;
	}
	#pragma endregion

	// Define multiple colour masks that are each used to detect a prominent single blob of homogenous color with a rectangular shape.
	std::vector<hex::Mask> maskDefintions = {
		hex::Mask("Red",       std::pair<cv::Scalar, cv::Scalar>(cv::Scalar(  0, 20, 20), cv::Scalar( 16, 255, 255)),
                               std::pair<cv::Scalar, cv::Scalar>(cv::Scalar(164, 20, 20), cv::Scalar(180, 255, 255))),
        hex::Mask("Orange",    std::pair<cv::Scalar, cv::Scalar>(cv::Scalar( 12, 20, 20), cv::Scalar( 24, 255, 255)),
                               std::pair<cv::Scalar, cv::Scalar>(cv::Scalar( 12, 20, 20), cv::Scalar( 24, 255, 255))),
        hex::Mask("Yellow",    std::pair<cv::Scalar, cv::Scalar>(cv::Scalar( 24, 20, 20), cv::Scalar( 32, 255, 255)),
                               std::pair<cv::Scalar, cv::Scalar>(cv::Scalar( 24, 20, 20), cv::Scalar( 32, 255, 255))),
        /*hex::Mask("Green",     std::pair<cv::Scalar, cv::Scalar>(cv::Scalar( 32, 20, 20), cv::Scalar( 75, 255, 255)),
                               std::pair<cv::Scalar, cv::Scalar>(cv::Scalar( 32, 20, 20), cv::Scalar( 75, 255, 255))),*/
        hex::Mask("LightBlue", std::pair<cv::Scalar, cv::Scalar>(cv::Scalar( 75, 20, 20), cv::Scalar(105, 255, 255)),
                               std::pair<cv::Scalar, cv::Scalar>(cv::Scalar( 75, 20, 20), cv::Scalar(105, 255, 255))),
        hex::Mask("DarkBlue",  std::pair<cv::Scalar, cv::Scalar>(cv::Scalar(105, 20, 20), cv::Scalar(130, 255, 255)),
                               std::pair<cv::Scalar, cv::Scalar>(cv::Scalar(105, 20, 20), cv::Scalar(130, 255, 255))),
        hex::Mask("Purple",    std::pair<cv::Scalar, cv::Scalar>(cv::Scalar(128, 20, 20), cv::Scalar(155, 255, 255)),
                               std::pair<cv::Scalar, cv::Scalar>(cv::Scalar(128, 20, 20), cv::Scalar(155, 255, 255))),
        hex::Mask("Magenta",   std::pair<cv::Scalar, cv::Scalar>(cv::Scalar(140, 20, 20), cv::Scalar(165, 255, 255)),
                               std::pair<cv::Scalar, cv::Scalar>(cv::Scalar(140, 20, 20), cv::Scalar(165, 255, 255)))
	};

    // Video Processing Loop
    while (1)
    {
        // Take the current frame of the video feed.
        cv::Mat frame; video >> frame;
        if (frame.empty()) break;

        // Process the current frame.
        auto result = processFrame(frame, maskDefintions);

        // Display the processing result.
        display(frame, result);

#ifdef TRAINING
        if (--loop_timer < 0)
        {
            loop_timer = loopTiming;
            WRITE_TRAINING_DATA()
        }
#endif

        // Close on `Escape` key press.
        char c = (char)cv::waitKey(1);
        if (c == 27) break;
    }

    // Clean up resources.
    video.release();
    cv::destroyAllWindows();
	return 0;
}

#pragma region Functions
static std::optional<hex::FrameResult> processFrame(cv::InputArray src, std::vector<hex::Mask>& maskDefinitions)
{
    // Scale the source image down for faster processing.
    cv::Mat smallSrc; cv::resize(src, smallSrc, cv::Size(), imageProcessingScale, imageProcessingScale);

    // Convert image to HSV color space for easier processing with masks.
    cv::Mat hsvSrc; cv::cvtColor(smallSrc, hsvSrc, cv::COLOR_BGR2HSV);

    std::vector<hex::MaskedImage> masks; getColourMasks(hsvSrc, maskDefinitions, masks);

    std::vector<std::future<hex::Processing>> searches;
    for (hex::MaskedImage mask : masks)
	{
        // TODO: See if it is possible to pass `cv::Mat`s by reference.
        searches.push_back(std::async(std::launch::async, [src, smallSrc, mask]() -> hex::Processing {
            auto start = std::chrono::high_resolution_clock::now();

		// Detect edges
        cv::Mat edges; cv::Canny(mask.maskImage, edges, 100, 200, 3);
		// Find contours
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(edges, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

        for (int i = 0; i < contours.size(); i++)
        {
            if (std::fabs(cv::contourArea(contours[i])) < 100)
                contours.erase(contours.begin() + i);
        }

		// Find the most prominent rectangular contour, and crop, and straighten the image to its bounds.
        cv::Mat cropped; std::optional<std::vector<cv::Point>> prominentContour = std::nullopt;
		for (int i = 0; i < contours.size(); i++)
		{
			if (isProminentRectangularContour(contours[i]))
			{
                cv::RotatedRect bounds = cv::minAreaRect(contours[i]);
				cropAndStraigthen(bounds, mask.maskImage, cropped);

                prominentContour = std::optional<std::vector<cv::Point>>(contours[i]);
			}
		}

            auto end = std::chrono::high_resolution_clock::now();

            return hex::Processing(
                prominentContour.has_value()
                    ? std::optional<hex::FrameResult>(hex::FrameResult(
                src.getMat(), mask.mask.colour, cropped,
                        hierarchy, contours, *prominentContour, smallSrc.size()))
                    : (std::optional<hex::FrameResult>)std::nullopt,
                // colour
                mask.mask.colour,
                // duration
                (end - start).count());
        }));
    }

    std::optional<hex::FrameResult> res = std::nullopt; unsigned ln = 1;
    for (std::future<hex::Processing>& search : searches)
    {
        auto processing = search.get();

        std::cout << line(ln++)
            << colour(143, processing.colour) << " processed in "
            << colour(143, std::to_string(processing.duration / 1000 / 1000))
            << "ms" << std::flush;

        if (processing.result.has_value()) res = processing.result;
    }
    return res;
}

static void display(cv::InputArray src, std::optional<hex::FrameResult> frame)
{
    cv::Mat srcMat = src.getMat();

    cv::namedWindow("Frame", cv::WINDOW_AUTOSIZE);

    if (frame.has_value())
    {
        // Draw contours
        cv::Mat drawing = cv::Mat::zeros(frame->processingSize, CV_8UC3);
        drawAllContours(drawing, frame->contours,                                                frame->hierarchy, (int)(contourThicknessScale * 2));
        drawAllContours(drawing, std::vector<std::vector<cv::Point>>{ frame->prominentContour }, frame->hierarchy, (int)(contourThicknessScale * 4));
        // Resize contours to the size of the source image.
        cv::Mat largeDrawing; cv::resize(drawing, largeDrawing, src.size(), 0, 0, cv::INTER_LINEAR);
        // Copy the contours on the camera feed.
        // TODO: Fix opacity
        srcMat += largeDrawing;

        // Convert the black & white result image into a coloured image so that it can later be copied to the screen.
        cv::Mat result; cv::cvtColor(frame->result, result, cv::COLOR_GRAY2RGB);
        double resultScaling = ((double)srcMat.rows * 2/5) / result.rows;
        // Resize the result image for it to be displayed on the screen.
        cv::Mat scaledResult; cv::resize(result, scaledResult, cv::Size(), resultScaling, resultScaling, cv::INTER_LINEAR);

        // Initialise screen
        cv::Mat screen = cv::Mat::zeros(cv::Size(srcMat.cols + scaledResult.cols, srcMat.rows), CV_8UC3);
        // Copy the camera feed and the result image to the screen.
        srcMat.copyTo(screen(cv::Rect(cv::Point(0, 0), srcMat.size())));
        scaledResult.copyTo(screen(cv::Rect(srcMat.cols, (screen.rows - scaledResult.rows) / 2, scaledResult.cols, scaledResult.rows)));

        // Colour Output
        cv::putText(screen, frame->colour, cv::Point(srcMat.cols, (screen.rows - scaledResult.rows + 48) / 2 + scaledResult.rows), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);

#ifdef TRAINING
        if (loop_timer <= 0)
            cv::circle(screen, cv::Point(screen.cols - 32, 32), 8, cv::Scalar(50, 50, 255), 16);
#endif

        // Display the modified camera feed with the results.
        cv::imshow("Frame", screen);
    }
    else
    {
        // Display the camera feed, unmodified.
        cv::imshow("Frame", srcMat);
    }
}

/// <summary>
/// Renders each colour mask for the specified source image.
/// </summary>
static void getColourMasks(cv::InputArray src, std::vector<hex::Mask> maskDefinitions, std::vector<hex::MaskedImage>& masks)
{
	std::vector<hex::MaskedImage> _masks;

	for (int i = 0; i < maskDefinitions.size(); i++)
		_masks.push_back(hex::MaskedImage::fromMask(maskDefinitions[i], src));

	masks = _masks;
}

/// <summary>
/// Draws an entire array of contours with the specified options.
/// </summary>
static void drawAllContours(cv::InputOutputArray& image, cv::InputArrayOfArrays contours, cv::InputArray hierarchy, int thickness, int lineType)
{
	cv::RNG rng = cv::RNG();
	cv::Scalar colour;

	for (int i = 0; i < contours.size().width; i++)
	{
		colour = cv::Scalar(rng.uniform(50, 255), rng.uniform(50, 255), rng.uniform(50, 255));
		cv::drawContours(image, contours, i, colour, thickness, lineType, hierarchy, 0, cv::Point());
	}
}

/// <summary>
/// Returns wether a contour defines a prominent, rectangular shape.
/// </summary>
static bool isProminentRectangularContour(std::vector<cv::Point> contour)
{
	// Approximate contours to polygons
	std::vector<cv::Point> approx;
	cv::approxPolyDP(cv::Mat(contour), approx, cv::arcLength(cv::Mat(contour), true) * 0.02, true);
	// Filter small contours
    if (std::fabs(cv::contourArea(contour)) < 100 || !cv::isContourConvex(approx)) return false;

	// Store the approximation polygons' number of verticies
	int nVerticies = approx.size();
	// Return if not quadrilateral
	if (nVerticies != 4) return false;

	// Get cosines of polygon
	std::vector<double> cos;
	for (int j = 2; j < nVerticies + 1; j++)
		cos.push_back(angle(approx[j % nVerticies], approx[j - 2], approx[j - 1]));
	auto minmaxcos = std::minmax_element(cos.begin(), cos.end());
	auto mincos = *minmaxcos.first; auto maxcos = *minmaxcos.second;

	// Check for rectangular contour
	return mincos >= -0.1 && maxcos <= 0.3;
}

/// <summary>
/// Transforms the source image and retreieves from it the area specified by the rotated rectangle bounds.
/// </summary>
static void cropAndStraigthen(cv::RotatedRect bounds, cv::InputArray input, cv::Mat& output)
{
	float angle = bounds.angle;
	cv::Size size = bounds.size;

	if (bounds.angle < -45.) {
		angle += 90.0;
		cv::swap(size.width, size.height);
	}
	cv::Mat rotationMatrix = cv::getRotationMatrix2D(bounds.center, angle, 1.0);
	// Perform the affine transformation on the original image
	cv::Mat rotated; cv::warpAffine(input, rotated, rotationMatrix, input.size(), cv::INTER_CUBIC);
	// Crop the rotated image to the original bounds
	cv::Mat cropped; cv::getRectSubPix(rotated, size, bounds.center, cropped);

	output = cropped;
}
#pragma endregion

#pragma region Helpers
/// <summary>
/// Creates a new window with the specified name, and displays the image on it.
/// </summary>
static void createWindow(cv::String name, cv::InputArray& image)
{
	cv::namedWindow(name, cv::WINDOW_KEEPRATIO);
	cv::imshow(name, image);
}
/// <summary>
/// Creates a new window with the specified name, and size, and displays the image on it.
/// </summary>
static void createWindow(cv::String name, cv::InputArray& image, int width, int height)
{
	createWindow(name, image);
	cv::resizeWindow(name, width, height);
}

static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x, dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x, dy2 = pt2.y - pt0.y;
	return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}
#pragma endregion
