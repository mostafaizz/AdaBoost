#pragma once
/// Abstract class to hold data
#include <opencv2\opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
class DataPoint
{
public:
	DataPoint();
	~DataPoint();
	virtual std::vector<double> getVectorData() = 0;
	virtual cv::Mat getMatData() = 0;
	virtual int getDimensions() = 0;
};

