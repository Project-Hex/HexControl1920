#pragma once
#include <opencv2/core.hpp>
#include <string>

namespace hex
{
    struct Mask
    {
        /// <summary>
        /// The name of the colour filtered by this mask.
        /// </summary>
        std::string colour;
        /// <summary>
        /// The high colour filter's colour range.
        /// </summary>
        std::pair<cv::Scalar, cv::Scalar> highMask;
        /// <summary>
        /// The low colour filter's colour range.
        /// </summary>
        std::pair<cv::Scalar, cv::Scalar> lowMask;

    public:
        Mask(std::string name, std::pair<cv::Scalar, cv::Scalar> highColourRange, std::pair<cv::Scalar, cv::Scalar> lowColourRange);

        /// <summary>
        /// Renders the colour mask for the specified source image.
        /// </summary>
        void render(cv::InputArray src, cv::Mat& mask, cv::Mat& out);
    };
}
