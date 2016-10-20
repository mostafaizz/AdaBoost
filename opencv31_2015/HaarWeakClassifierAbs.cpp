#include "HaarWeakClassifierAbs.h"



HaarWeakClassifierAbs::HaarWeakClassifierAbs(cv::Size sz, cv::Point pt , std::vector<std::vector<int> > sh, int direction):
	HaarWeakClassifier(sz,pt,sh,direction)
{
	dummy = 1;
}

int HaarWeakClassifierAbs::classify(DataPoint * data)
{
	HaarWeakClassifier::classify(data);
	this->data = std::abs(this->data);
	//if (this->data)
	//{
	//	std::cout << this->data << std::endl;
	//}
	return WeakClassifier::classify();
}

bool HaarWeakClassifierAbs::operator==(WeakClassifier * obj)
{
	if (dynamic_cast<HaarWeakClassifierAbs*>(obj))
	{
		if (HaarWeakClassifier::operator==(obj))
		{
			return true;
		}
	}
	return false;
}

void HaarWeakClassifierAbs::operator=(const WeakClassifier & obj)
{
	HaarWeakClassifierAbs* ptr = (HaarWeakClassifierAbs*)&obj;
	this->data = ptr->data;
	this->edge = ptr->edge;
	this->featureIndex = ptr->featureIndex;
	this->location = ptr->location;
	this->dummy = ptr->dummy;
}

std::string HaarWeakClassifierAbs::getName()
{
	return "HaarWeakClassifierAbs";
}

