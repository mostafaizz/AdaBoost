#pragma once
#include "AdaBoost.h"
#include "MatData.h"
#include "WeakHaarClassifierFactory.h"
#include <opencv2/core.hpp>

class AdaBoostEdgeDetector
{
	int numClassifiers;
	cv::Size patchSize;
	int step;
	WeakHaarClassifierFactory * factory;
	AdaBoost* adaBoost;

public:
	static cv::Mat cannyEdgeDetection(cv::Mat m);
	
	AdaBoostEdgeDetector(int numClassifiers, std::vector<std::vector<std::vector<int>>>& shapes, cv::Size patchSize, int step);
	std::vector<cv::Mat> train(std::vector<cv::Mat> images, bool display = false);
	cv::Mat test(cv::Mat img, bool display = false);
	~AdaBoostEdgeDetector();
};

