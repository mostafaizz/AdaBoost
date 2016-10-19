#pragma once
#include "DataPoint.h"
class MatData :
	public DataPoint
{
	cv::Mat data;
public:
	MatData();
	virtual std::vector<double> getVectorData();
	virtual cv::Mat getMatData();
	virtual int getDimensions();
	~MatData();
};

