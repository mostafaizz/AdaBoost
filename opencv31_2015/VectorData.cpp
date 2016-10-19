#include "VectorData.h"



VectorData::VectorData(std::vector<double> & d)
{
	data = d;
}

std::vector<double> VectorData::getVectorData()
{
	return data;
}

cv::Mat VectorData::getMatData()
{
	throw "Not Implemented";
	return cv::Mat();
}


VectorData::~VectorData()
{
}
