#pragma once
#include "DataPoint.h"
#include <vector>
class VectorData :
	public DataPoint
{
	std::vector<double> data;
public:
	
	VectorData(std::vector<double> &);
	virtual std::vector<double> getVectorData();
	virtual cv::Mat getMatData();
	virtual int getDimensions();
	~VectorData();
};

