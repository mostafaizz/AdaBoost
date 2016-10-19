#include "WeakHaarClassifierFactory.h"
#include "HaarWeakClassifier.h"


WeakHaarClassifierFactory::WeakHaarClassifierFactory(std::vector<std::vector<std::vector<int>>>& shapes, 
	std::vector<cv::Size>& sizes,
	std::vector<cv::Point>& positions)
{
	this->shapes = shapes;
	this->sizes = sizes;
	this->positions = positions;
}

std::vector<WeakClassifier*> WeakHaarClassifierFactory::getClassifiers(DataPoint *data)
{
	std::vector<WeakClassifier*> res;
	for (std::vector<std::vector<std::vector<int> > >::iterator sh = shapes.begin(); sh != shapes.end(); sh++)
	{
		for (std::vector<cv::Size>::iterator sz = sizes.begin(); sz != sizes.end(); sz++)
		{
			for (std::vector<cv::Point>::iterator pt = positions.begin(); pt != positions.end(); pt++)
			{
				HaarWeakClassifier * w = new HaarWeakClassifier(*sz, *pt, *sh, 1);
				HaarWeakClassifier * w1 = new HaarWeakClassifier(*sz, *pt, *sh, -1);
				w->updateEdgewithClassification(data);
				w1->updateEdgewithClassification(data);
				res.push_back(w);
				res.push_back(w1);
			}
		}
	}
	return res;
}

WeakHaarClassifierFactory::~WeakHaarClassifierFactory()
{
}
