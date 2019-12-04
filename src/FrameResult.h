#pragma once
#include <opencv2/core.hpp>
#include <string>

namespace hex
{
    struct FrameResult
    {
        cv::Mat src;

        std::vector<cv::Vec4i> hierarchy;
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Point> prominentContour;
        cv::Size processingSize;

        std::string colour;
        cv::Mat result;

    public:
        FrameResult(
            cv::Mat src, std::string colour, cv::Mat result,
            std::vector<cv::Vec4i> hierarchy,
            std::vector<std::vector<cv::Point>> contours,
            std::vector<cv::Point> prominentContour,
            cv::Size processingSize);
    };
}
