#pragma once
#include <opencv2/core.hpp>
#include <string>
using namespace std;
using namespace cv;

struct FrameResult
{
    Mat src;

    vector<Vec4i> hierarchy;
    vector<vector<Point>> contours;
    vector<Point> prominentContour;
    Size processingSize;

    string colour;
    Mat result;

public:
    FrameResult(Mat src, string colour, Mat result, vector<Vec4i> hierarchy, vector<vector<Point>> contours, vector<Point> prominentContour, Size processingSize);
};
