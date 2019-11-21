#pragma once
#include <opencv2/core.hpp>
#include "Mask.h"
using namespace cv;

struct MaskedImage
{
	/// <summary>
	/// The mask used to create this masked image.
	/// </summary>
	Mask mask;
	/// <summary>
	/// The image used to create this masked image.
	/// </summary>
	Mat src;
	/// <summary>
	/// The image depicting the mask.
	/// </summary>
	Mat maskImage;
	/// <summary>
	/// The image depicting the source image masked.
	/// </summary>
	Mat maskedImage;

public:
	MaskedImage(Mask mask, Mat src, Mat maskImage, Mat maskedImage);

	/// <summary>
	/// Creates a masked image from the specified mask and source image.
	/// </summary>
	static MaskedImage fromMask(Mask mask, InputArray src);
};