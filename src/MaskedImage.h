#pragma once
#include <opencv2/core.hpp>
#include "Mask.h"

namespace hex
{
    struct MaskedImage
    {
        /// <summary>
        /// The mask used to create this masked image.
        /// </summary>
        Mask mask;
        /// <summary>
        /// The image used to create this masked image.
        /// </summary>
        cv::Mat src;
        /// <summary>
        /// The image depicting the mask.
        /// </summary>
        cv::Mat maskImage;
        /// <summary>
        /// The image depicting the source image masked.
        /// </summary>
        cv::Mat maskedImage;

    public:
        MaskedImage(Mask mask, cv::Mat src, cv::Mat maskImage, cv::Mat maskedImage);

        /// <summary>
        /// Creates a masked image from the specified mask and source image.
        /// </summary>
        static MaskedImage fromMask(Mask mask, cv::InputArray src);
    };
}
