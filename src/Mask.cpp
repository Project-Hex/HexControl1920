#include <opencv2/core.hpp>
#include "Mask.h"

namespace hex
{
    Mask::Mask(std::string name, std::pair<cv::Scalar, cv::Scalar> highColourRange, std::pair<cv::Scalar, cv::Scalar> lowColourRange)
        : colour(name), highMask(highColourRange), lowMask(lowColourRange) { }

    /// <summary>
    /// Renders the colour mask for the specified source image.
    /// </summary>
    void Mask::render(cv::InputArray src, cv::Mat& mask, cv::Mat& out)
    {
        cv::Mat mask_high, mask_low;
        inRange(src, highMask.first, highMask.second, mask_high);
        inRange(src, lowMask.first, lowMask.second, mask_low);

        mask = mask_high + mask_low;
        bitwise_or(src, src, out, mask);
    }
}
