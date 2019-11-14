#pragma once
#include <opencv2/core.hpp>
#include <iostream>
using namespace std;
using namespace cv;

struct Mask
{
	/// <summary>
	/// The name of the colour filtered by this mask.
	/// </summary>
	string colour;
	/// <summary>
	/// The high colour filter's colour range.
	/// </summary>
	pair<Scalar, Scalar> highMask;
	/// <summary>
	/// The low colour filter's colour range.
	/// </summary>
	pair<Scalar, Scalar> lowMask;

public:
	Mask(string name, pair<Scalar, Scalar> highColourRange, pair<Scalar, Scalar> lowColourRange);

	/// <summary>
	/// Renders the colour mask for the specified source image.
	/// </summary>
	void render(InputArray src, Mat& mask, Mat& out);
};
