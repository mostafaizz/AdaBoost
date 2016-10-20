#include "WeakHaarClassifierFactory.h"
#include "HaarWeakClassifier.h"
#include "HaarWeakClassifierAbs.h"


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
				int num = 0;
				/*
				for (int dir = -1; dir <= 1; dir += 2)
				{
					HaarWeakClassifier * w = new HaarWeakClassifier(*sz, *pt, *sh, dir);
					if (w->updateEdgewithClassification(data))
					{
						res.push_back(w);
						num++;
					}
				}
				*/
				for (int dir = -1; dir <= 1; dir += 2)
				{
					HaarWeakClassifierAbs * w = new HaarWeakClassifierAbs(*sz, *pt, *sh, dir);
					if (w->updateEdgewithClassification(data))
					{
						res.push_back(w);
						num++;
					}
				}
			}
		}
	}
	return res;
}

WeakClassifier * WeakHaarClassifierFactory::copyClassifier(WeakClassifier * w)
{
	WeakClassifier * w1;
	if (dynamic_cast<HaarWeakClassifierAbs*>(w))
	{
		w1 = new HaarWeakClassifierAbs(*((HaarWeakClassifierAbs*)w));
	}
	else if (dynamic_cast<HaarWeakClassifier*>(w))
	{
		w1 = new HaarWeakClassifier(*((HaarWeakClassifier*)w));
	}
	return w1;
}

WeakHaarClassifierFactory::~WeakHaarClassifierFactory()
{
}
