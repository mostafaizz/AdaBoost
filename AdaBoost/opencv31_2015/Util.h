#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
class Util
{
public:
	Util();
	static double getRectangleSum(cv::Mat& iImg, cv::Rect win);
	static void displayIntegralImage(cv::Mat Iimg);

	~Util();
};

