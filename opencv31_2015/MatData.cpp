#include "MatData.h"



MatData::MatData(cv::Mat d)
{
	data = d;
}

std::vector<double> MatData::getVectorData()
{
	throw "getVectorData Not implemented for this class";
	return std::vector<double>();
}

cv::Mat MatData::getMatData()
{
	return data;
}

int MatData::getDimensions()
{
	return data.dims;
}


MatData::~MatData()
{
}
