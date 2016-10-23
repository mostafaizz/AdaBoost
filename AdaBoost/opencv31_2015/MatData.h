#pragma once
#include "DataPoint.h"
class MatData :
	public DataPoint
{
	cv::Mat data;
	cv::Rect roi;
public:
	MatData(cv::Mat, cv::Rect);
	cv::Rect getROI();
	virtual std::vector<double> getVectorData();
	virtual cv::Mat getMatData();
	virtual int getDimensions();
	~MatData();
};

