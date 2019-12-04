#include <opencv2/core.hpp>
#include <string>
#include "FrameResult.h"
using namespace std;
using namespace cv;

FrameResult::FrameResult(Mat src, string colour, Mat result, vector<Vec4i> hierarchy, vector<vector<Point>> contours, vector<Point> prominentContour, Size processingSize)
    : src(src), colour(colour), result(result), hierarchy(hierarchy), contours(contours), prominentContour(prominentContour), processingSize(processingSize) { }
