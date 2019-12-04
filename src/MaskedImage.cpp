#include <opencv2/core.hpp>
#include "Mask.h"
#include "MaskedImage.h"

namespace hex
{
    MaskedImage::MaskedImage(Mask mask, cv::Mat src, cv::Mat maskImage, cv::Mat maskedImage)
        : mask(mask), src(src), maskImage(maskImage), maskedImage(maskedImage) { }

    /// <summary>
    /// Creates a masked image from the specified mask and source image.
    /// </summary>
    MaskedImage MaskedImage::fromMask(Mask mask, cv::InputArray src)
    {
        cv::Mat maskImage, maskedImage;
        mask.render(src, maskImage, maskedImage);
        return MaskedImage(mask, src.getMat(), maskImage, maskedImage);
    }
}
