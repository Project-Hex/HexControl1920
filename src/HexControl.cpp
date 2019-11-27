#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdarg.h>
#include "Mask.h"
#include "MaskedImage.h"
using namespace cv;
using namespace std;

#pragma region Functions
static void processFrame(InputArray src, vector<Mask>& maskDefinitions);
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
	// Check for valid argument count
	if (argc != 2)
	{
		cout << " Usage: HexControl.exe image" << endl;
		return -1;
	}

	// Read input file
	Mat src = imread(argv[1], IMREAD_COLOR);
	// Check for invalid input
	if (src.empty())
	{
		cout << "Could not open or find the image" << endl;
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

    processFrame(src, maskDefintions);

	waitKey(0);
	return 0;
}

#pragma region Functions
static void processFrame(InputArray src, vector<Mask>& maskDefinitions)
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

		// Draw contours
		Mat drawing = Mat::zeros(edges.size(), CV_8UC3); drawAllContours(drawing, contours, hierarchy, contourThicknessScale * 2);

		// Find the most prominent rectangular contour, and crop, and straighten the image to its bounds.
        Mat cropped; int nProminentContours = 0;
		for (int i = 0; i < contours.size(); i++)
		{
			if (isProminentRectangularContour(contours[i]))
			{
                drawAllContours(drawing, vector<vector<Point>>{ contours[i] }, hierarchy, contourThicknessScale * 4);

				RotatedRect bounds = minAreaRect(contours[i]);
				cropAndStraigthen(bounds, mask.maskImage, cropped);

                nProminentContours++;
			}
		}

        if (nProminentContours == 0)
            continue;

		// Display images
		createWindow("Contours - " + mask.mask.colour, drawing, 1024, 768);
		createWindow("Crop - " + mask.mask.colour,     cropped);
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
