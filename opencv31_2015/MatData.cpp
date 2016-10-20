#include "MatData.h"


MatData::MatData(cv::Mat d, cv::Rect r)
{
	data = d;
	roi = r;
}

std::vector<double> MatData::getVectorData()
{
	throw "getVectorData Not implemented for this class";
	return std::vector<double>();
}

cv::Mat MatData::getMatData()
{
	return data(roi);
}

int MatData::getDimensions()
{
	return data.dims;
}


MatData::~MatData()
{
}
