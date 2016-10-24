#pragma once
#include "AdaBoost.h"
#include "MatData.h"
#include "WeakHaarClassifierFactory.h"
#include <opencv2/core.hpp>

class AdaBoostCascadeClassifier
{
protected:
	std::vector<int> numClassifiers;
	cv::Size patchSize;
	int step;
	WeakHaarClassifierFactory * factory;
	std::vector<AdaBoost*> adaBoostLevels;
	DataPoint* getNormalizedData(cv::Mat & Iimg, cv::Mat & I2img, cv::Rect& roi);
public:
	AdaBoostCascadeClassifier(std::vector<int> numClassifiers,
		std::vector<std::vector<std::vector<int>>>& shapes, cv::Size patchSize, int step);
	std::vector<int> train(std::vector<cv::Mat>& images, std::vector<int>& labels);
	std::vector<cv::Rect> test(cv::Mat& img, std::vector<double> sizeFactors);
	~AdaBoostCascadeClassifier();
};