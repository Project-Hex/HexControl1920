#include <opencv2/core.hpp>
#include "Mask.h"
#include "MaskedImage.h"
using namespace cv;

MaskedImage::MaskedImage(Mask mask, Mat src, Mat maskImage, Mat maskedImage)
	: mask(mask), src(src), maskImage(maskImage), maskedImage(maskedImage) { }

/// <summary>
/// Creates a masked image from the specified mask and source image.
/// </summary>
MaskedImage MaskedImage::fromMask(Mask mask, InputArray src)
{
	Mat maskImage, maskedImage;
	mask.render(src, maskImage, maskedImage);
	return MaskedImage(mask, src.getMat(), maskImage, maskedImage);
}