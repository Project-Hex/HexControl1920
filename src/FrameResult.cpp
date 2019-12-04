#include <opencv2/core.hpp>
#include <string>
#include "FrameResult.h"

namespace hex
{
    FrameResult::FrameResult(
        cv::Mat src, std::string colour, cv::Mat result,
        std::vector<cv::Vec4i> hierarchy,
        std::vector<std::vector<cv::Point>> contours,
        std::vector<cv::Point> prominentContour,
        cv::Size processingSize)
        : src(src), colour(colour), result(result),
          hierarchy(hierarchy),
          contours(contours),
          prominentContour(prominentContour),
          processingSize(processingSize) { }
}
