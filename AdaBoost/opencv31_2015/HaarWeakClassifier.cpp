#include "HaarWeakClassifier.h"
#include "Util.h"


HaarWeakClassifier::HaarWeakClassifier(cv::Size sz, cv::Point location, std::vector<std::vector<int> >shape,int direction) :WeakClassifier(0, 0, direction)
{
	this->origSize = sz;
	this->origLocation = location;
	this->shape = shape;
}

// assuming integral float image
int HaarWeakClassifier::classify(DataPoint * d, double sizeFactor)
{
	cv::Point location = origLocation * sizeFactor;
	cv::Size size(sizeFactor * origSize.width, sizeFactor * origSize.height);

	// this should return an integral image
	cv::Mat m = d->getMatData();
	// check if the filter fits inside
	if (m.rows < location.y + shape.size() * size.height)
		return -1;
	if (m.cols < location.x + shape[0].size() * size.width)
		return -1;
	if (size.height * size.width == 0)
		return -1;

	data = 0;
	for (int r = 0; r < shape.size(); r++)
	{
		for (int c = 0; c < shape[r].size(); c++)
		{
			//double A = m.at<float>(, );
			//double B = m.at<float>(location.y + (r + 1) * size.height - 1, location.x + c * size.width);
			//double C = m.at<float>(location.y + r * size.height, location.x + (c + 1) * size.width - 1);
			//double D = m.at<float>(location.y + r * size.height, location.x + c * size.width);

			//double tmp = A - B - C + D;

			double tmp = Util::getRectangleSum(m, cv::Rect(location.x + c * size.width,
				location.y + r * size.height,
				size.width, size.height));

			tmp *= shape[r][c];
			data += tmp;
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
	this->origLocation = ptr->origLocation;
}

bool HaarWeakClassifier::operator==(WeakClassifier* obj)
{
	HaarWeakClassifier* tmp = (HaarWeakClassifier*)obj;
	if (origSize != tmp->origSize)
	{
		return false;
	}
	if (origLocation != tmp->origLocation)
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

void HaarWeakClassifier::print()
{
	WeakClassifier::print();
	std::cout << " size: " << origSize << " loc: " << origLocation << " shape: ";
	for (int i = 0; i < shape.size(); i++)
	{
		std::cout <<  std::endl << "\t";
		for (int j = 0; j < shape[i].size(); j++)
		{
			std::cout << shape[i][j] << "\t";
		}
	}
}

std::string HaarWeakClassifier::getName()
{
	return "HaarWeakClassifier";
}

HaarWeakClassifier::~HaarWeakClassifier()
{
}
