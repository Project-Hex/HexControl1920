#pragma once
#include <opencv2/core.hpp>
#include <string>
using namespace std;
using namespace cv;

struct FrameResult
{
    Mat src;

    string colour;
    Mat contours;
    Mat result;

public:
    FrameResult(Mat src, string colour, Mat contours, Mat result);
};

