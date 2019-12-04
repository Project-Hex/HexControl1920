#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdarg.h>
#include <optional>
#include "Mask.h"
#include "MaskedImage.h"
#include "FrameResult.h"
using namespace cv;
using namespace std;

#pragma region Functions
static optional<FrameResult> processFrame(InputArray src, vector<Mask>& maskDefinitions);
static void display(InputArray src, optional<FrameResult> frame);
static void getColourMasks(InputArray src, vector<Mask> maskDefinitions, vector<MaskedImage>& masks);
static void drawAllContours(InputOutputArray& image, InputArrayOfArrays contours, InputArray hierarchy, int thickness = 1, int lineType = LINE_8);
static bool isProminentRectangularContour(vector<Point> contour);
static void cropAndStraigthen(RotatedRect bounds, InputArray input, Mat& output);
#pragma endregion

#pragma region Helpers
static void createWindow(String name, InputArray& image);
static void createWindow(String name, InputArray& image, int width, int height);
static double angle(Point pt1, Point pt2, Point pt0);
#pragma endregion

constexpr double imageProcessingScale = 0.2;
constexpr double contourThicknessScale = 1;

int main(int argc, char** argv)
{
	#pragma region Input
    // Create the video capture feed for interacting with the camera.
    VideoCapture video(0);

    // Check if the camera has been opened successfully.
    if (!video.isOpened())
	{
        cout << "Error opening video stream or file" << endl;
		return -1;
	}
	#pragma endregion

	// Define multiple colour masks that are each used to detect a prominent single blob of homogenous color with a rectangular shape.
	vector<Mask> maskDefintions = {
		Mask("Red",         pair<Scalar, Scalar>(Scalar(  0, 20, 20), Scalar( 16, 255, 255)), pair<Scalar, Scalar>(Scalar(164, 20, 20), Scalar(180, 255, 255))),
        Mask("Orange",      pair<Scalar, Scalar>(Scalar( 12, 20, 20), Scalar( 24, 255, 255)), pair<Scalar, Scalar>(Scalar( 12, 20, 20), Scalar( 24, 255, 255))),
        Mask("Yellow",      pair<Scalar, Scalar>(Scalar( 24, 20, 20), Scalar( 32, 255, 255)), pair<Scalar, Scalar>(Scalar( 24, 20, 20), Scalar( 32, 255, 255))),
        //Mask("Green",       pair<Scalar, Scalar>(Scalar( 32, 20, 20), Scalar( 75, 255, 255)), pair<Scalar, Scalar>(Scalar( 32, 20, 20), Scalar( 75, 255, 255))),
        Mask("LightBlue",   pair<Scalar, Scalar>(Scalar( 75, 20, 20), Scalar(105, 255, 255)), pair<Scalar, Scalar>(Scalar( 75, 20, 20), Scalar(105, 255, 255))),
        Mask("DarkBlue",    pair<Scalar, Scalar>(Scalar(105, 20, 20), Scalar(130, 255, 255)), pair<Scalar, Scalar>(Scalar(105, 20, 20), Scalar(130, 255, 255))),
        Mask("Purple",      pair<Scalar, Scalar>(Scalar(128, 20, 20), Scalar(155, 255, 255)), pair<Scalar, Scalar>(Scalar(128, 20, 20), Scalar(155, 255, 255))),
        Mask("Magenta",     pair<Scalar, Scalar>(Scalar(140, 20, 20), Scalar(165, 255, 255)), pair<Scalar, Scalar>(Scalar(140, 20, 20), Scalar(165, 255, 255)))
	};

    // Video Processing Loop
    while (1)
    {
        // Take the current frame of the video feed.
        Mat frame; video >> frame;
        if (frame.empty()) break;

        // Process the current frame.
        auto result = processFrame(frame, maskDefintions);

        // Display the processing result.
        display(frame, result);

        // Close on `Escape` key press.
        char c = (char)waitKey(1);
        if (c == 27) break;
    }

    // Clean up resources.
    video.release();
    destroyAllWindows();
	return 0;
}

#pragma region Functions
static optional<FrameResult> processFrame(InputArray src, vector<Mask>& maskDefinitions)
{
    // Scale the source image down for faster processing.
    Mat smallSrc; resize(src, smallSrc, Size(), imageProcessingScale, imageProcessingScale);

    // Convert image to HSV color space for easier processing with masks.
    Mat hsvSrc; cvtColor(smallSrc, hsvSrc, COLOR_BGR2HSV);

    vector<MaskedImage> masks; getColourMasks(hsvSrc, maskDefinitions, masks);

	for (MaskedImage mask : masks)
	{
		// Detect edges
		Mat edges; Canny(mask.maskImage, edges, 100, 200, 3);
		// Find contours
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(edges, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

        for (int i = 0; i < contours.size(); i++)
        {
            if (fabs(contourArea(contours[i])) < 100)
                contours.erase(contours.begin() + i);
        }

		// Find the most prominent rectangular contour, and crop, and straighten the image to its bounds.
        Mat cropped; optional<vector<Point>> prominentContour = nullopt;
		for (int i = 0; i < contours.size(); i++)
		{
			if (isProminentRectangularContour(contours[i]))
			{
				RotatedRect bounds = minAreaRect(contours[i]);
				cropAndStraigthen(bounds, mask.maskImage, cropped);

                prominentContour = optional<vector<Point>>(contours[i]);
			}
		}

        if (prominentContour.has_value())
            return optional<FrameResult>(FrameResult(src.getMat(), mask.mask.colour, cropped, hierarchy, contours, *prominentContour, smallSrc.size()));
    }

    return nullopt;
}

static void display(InputArray src, optional<FrameResult> frame)
{
    Mat srcMat = src.getMat();

    namedWindow("Frame", WINDOW_AUTOSIZE);

    if (frame.has_value())
    {
        // Draw contours
        Mat drawing = Mat::zeros(frame->processingSize, CV_8UC3);
        drawAllContours(drawing, frame->contours,                                  frame->hierarchy, (int)(contourThicknessScale * 2));
        drawAllContours(drawing, vector<vector<Point>>{ frame->prominentContour }, frame->hierarchy, (int)(contourThicknessScale * 4));
        // Resize contours to the size of the source image.
        Mat largeDrawing; resize(drawing, largeDrawing, src.size(), 0, 0, INTER_LINEAR);
        // Copy the contours on the camera feed.
        // TODO: Fix opacity
        srcMat += largeDrawing;

        // Convert the black & white result image into a coloured image so that it can later be copied to the screen.
        Mat result; cvtColor(frame->result, result, COLOR_GRAY2RGB);
        double resultScaling = ((double)srcMat.rows * 2/5) / result.rows;
        // Resize the result image for it to be displayed on the screen.
        Mat scaledResult; resize(result, scaledResult, Size(), resultScaling, resultScaling, INTER_LINEAR);

        // Initialise screen
        Mat screen = Mat::zeros(Size(srcMat.cols + scaledResult.cols, srcMat.rows), CV_8UC3);
        // Copy the camera feed and the result image to the screen.
        srcMat.copyTo(screen(Rect(Point(0, 0), srcMat.size())));
        scaledResult.copyTo(screen(Rect(srcMat.cols, (screen.rows - scaledResult.rows) / 2, scaledResult.cols, scaledResult.rows)));

        // Colour Output
        putText(screen, frame->colour, Point(srcMat.cols, (screen.rows - scaledResult.rows + 48) / 2 + scaledResult.rows), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);

        // Display the modified camera feed with the results.
        imshow("Frame", screen);
    }
    else
    {
        // Display the camera feed, unmodified.
        imshow("Frame", srcMat);
    }
}

/// <summary>
/// Renders each colour mask for the specified source image.
/// </summary>
static void getColourMasks(InputArray src, vector<Mask> maskDefinitions, vector<MaskedImage>& masks)
{
	vector<MaskedImage> _masks;

	for (int i = 0; i < maskDefinitions.size(); i++)
		_masks.push_back(MaskedImage::fromMask(maskDefinitions[i], src));

	masks = _masks;
}

/// <summary>
/// Draws an entire array of contours with the specified options.
/// </summary>
static void drawAllContours(InputOutputArray& image, InputArrayOfArrays contours, InputArray hierarchy, int thickness, int lineType)
{
	RNG rng = RNG();
	Scalar colour;

	for (int i = 0; i < contours.size().width; i++)
	{
		colour = Scalar(rng.uniform(50, 255), rng.uniform(50, 255), rng.uniform(50, 255));
		drawContours(image, contours, i, colour, thickness, lineType, hierarchy, 0, Point());
	}
}

/// <summary>
/// Returns wether a contour defines a prominent, rectangular shape.
/// </summary>
static bool isProminentRectangularContour(vector<Point> contour)
{
	// Approximate contours to polygons
	vector<Point> approx;
	approxPolyDP(Mat(contour), approx, arcLength(Mat(contour), true) * 0.02, true);
	// Filter small contours
	if (fabs(contourArea(contour)) < 100 || !isContourConvex(approx)) return false;

	// Store the approximation polygons' number of verticies
	int nVerticies = approx.size();
	// Return if not quadrilateral
	if (nVerticies != 4) return false;

	// Get cosines of polygon
	vector<double> cos;
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
static void cropAndStraigthen(RotatedRect bounds, InputArray input, Mat& output)
{
	float angle = bounds.angle;
	Size size = bounds.size;

	if (bounds.angle < -45.) {
		angle += 90.0;
		swap(size.width, size.height);
	}
	Mat rotationMatrix = getRotationMatrix2D(bounds.center, angle, 1.0);
	// Perform the affine transformation on the original image
	Mat rotated; warpAffine(input, rotated, rotationMatrix, input.size(), INTER_CUBIC);
	// Crop the rotated image to the original bounds
	Mat cropped; getRectSubPix(rotated, size, bounds.center, cropped);

	output = cropped;
}
#pragma endregion

#pragma region Helpers
/// <summary>
/// Creates a new window with the specified name, and displays the image on it.
/// </summary>
static void createWindow(String name, InputArray& image)
{
	namedWindow(name, WINDOW_KEEPRATIO);
	imshow(name, image);
}
/// <summary>
/// Creates a new window with the specified name, and size, and displays the image on it.
/// </summary>
static void createWindow(String name, InputArray& image, int width, int height)
{
	createWindow(name, image);
	resizeWindow(name, width, height);
}

static double angle(Point pt1, Point pt2, Point pt0)
{
	double dx1 = pt1.x - pt0.x, dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x, dy2 = pt2.y - pt0.y;
	return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}
#pragma endregion
