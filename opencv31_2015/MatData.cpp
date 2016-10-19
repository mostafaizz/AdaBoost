#include "MatData.h"



MatData::MatData()
{
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


MatData::~MatData()
{
}
