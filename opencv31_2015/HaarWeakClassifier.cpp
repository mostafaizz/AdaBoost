#include "HaarWeakClassifier.h"



HaarWeakClassifier::HaarWeakClassifier(cv::Size sz, cv::Point location, std::vector<std::vector<int> >shape,int direction) :WeakClassifier(0, 0, direction)
{
	this->size = sz;
	this->location = location;
	this->shape = shape;
}

// assuming integral float image
int HaarWeakClassifier::classify(DataPoint * d)
{
	// this should return an integral image
	cv::Mat m = d->getMatData();
	// check if the filter fits inside
	if (m.rows < location.y + shape.size() * size.height)
		return 0;
	if (m.cols < location.x + shape[0].size() * size.width)
		return 0;
	data = 0;
	for (int r = 0; r < shape.size(); r++)
	{
		for (int c = 0; c < shape[r].size(); c++)
		{
			double A = m.at<float>(location.y + (r + 1) * size.height - 1, location.x + (c + 1) * size.width - 1);
			double B = m.at<float>(location.y + (r + 1) * size.height - 1, location.x + c * size.width);
			double C = m.at<float>(location.y + r * size.height, location.x + (c + 1) * size.width - 1);
			double D = m.at<float>(location.y + r * size.height, location.x + c * size.width);

			data += ((A - B - C + D) * shape[r][c]);
		}
	}
	return WeakClassifier::classify();
}

void HaarWeakClassifier::operator=(const WeakClassifier & obj)
{
	HaarWeakClassifier* ptr = (HaarWeakClassifier*)&obj;
	this->data = ptr->data;
	this->edge = ptr->edge;
	this->featureIndex = ptr->featureIndex;
	this->location = ptr->location;
}

bool HaarWeakClassifier::operator==(WeakClassifier* obj)
{
	HaarWeakClassifier* tmp = (HaarWeakClassifier*)obj;
	if (size != tmp->size)
	{
		return false;
	}
	if (location != tmp->location)
	{
		return false;
	}
	if (shape.size() == tmp->shape.size())
	{
		for (int i = 0; i < shape.size(); i++)
		{
			if (shape[i].size() != tmp->shape[i].size())
			{
				return false;
			}
			for (int j = 0; j < shape[i].size(); j++)
			{
				if (shape[i][j] != tmp->shape[i][j])
				{
					return false;
				}
			}
		}
	}
	else
	{
		return false;
	}
	if ((WeakClassifier(*this)) == obj)
	{
		return true;
	}

	return false;
}

HaarWeakClassifier::~HaarWeakClassifier()
{
}
