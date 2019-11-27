#include <opencv2/core.hpp>
#include <string>
#include "FrameResult.h"
using namespace std;
using namespace cv;

FrameResult::FrameResult(Mat src, string colour, Mat contours, Mat result)
    : src(src), colour(colour), contours(contours), result(result) { }
