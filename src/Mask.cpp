#include <opencv2/core.hpp>
#include "Mask.h"
using namespace std;
using namespace cv;

Mask::Mask(string name, pair<Scalar, Scalar> highColourRange, pair<Scalar, Scalar> lowColourRange)
	: colour(name), highMask(highColourRange), lowMask(lowColourRange) { }

/// <summary>
/// Renders the colour mask for the specified source image.
/// </summary>
void Mask::render(InputArray src, Mat& mask, Mat& out)
{
	Mat mask_high, mask_low;
	inRange(src, highMask.first, highMask.second, mask_high);
	inRange(src, lowMask.first, lowMask.second, mask_low);

	mask = mask_high + mask_low;
	bitwise_or(src, src, out, mask);
}
