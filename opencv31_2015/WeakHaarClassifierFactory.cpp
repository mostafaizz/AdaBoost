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
				HaarWeakClassifier * w = new HaarWeakClassifier(*sz, *pt, *sh, 1);
				HaarWeakClassifier * w1 = new HaarWeakClassifier(*sz, *pt, *sh, -1);
				HaarWeakClassifierAbs *w2 = new HaarWeakClassifierAbs(*sz, *pt, *sh, 1);
				HaarWeakClassifierAbs *w3 = new HaarWeakClassifierAbs(*sz, *pt, *sh, -1);
				w->updateEdgewithClassification(data);
				w1->updateEdgewithClassification(data);
				w2->updateEdgewithClassification(data);
				w3->updateEdgewithClassification(data);
				if (!flags[w->getData()])
				{
					//res.push_back(w);
					//res.push_back(w1);
					res.push_back(w2);
					//res.push_back(w3);
					flags[w->getData()] = 1;
					//flags[w2->getData()] = 1;
				}
			}
		}
	}
	/*if (flags.size() > 1)
	{
		std::vector<double> thrs;
		for (std::map<double, int>::iterator itr = flags.begin(); itr != flags.end(); itr++)
		{
			thrs.push_back(itr->first);
		}
		for (int i = 1; i < thrs.size(); i++)
		{
			double thr = (thrs[i] + thrs[i - 1]) / 2.0;
			for (std::vector<std::vector<std::vector<int> > >::iterator sh = shapes.begin(); sh != shapes.end(); sh++)
			{
				for (std::vector<cv::Size>::iterator sz = sizes.begin(); sz != sizes.end(); sz++)
				{
					for (std::vector<cv::Point>::iterator pt = positions.begin(); pt != positions.end(); pt++)
					{

						HaarWeakClassifier * w = new HaarWeakClassifier(*sz, *pt, *sh, 1);
						w->setEdge(thr);
						HaarWeakClassifier * w1 = new HaarWeakClassifier(*sz, *pt, *sh, -1);
						w1->setEdge(thr);
						res.push_back(w);
						res.push_back(w1);
					}
				}
			}
		}
	}*/
	return res;
}

WeakClassifier * WeakHaarClassifierFactory::copyClassifier(WeakClassifier * w)
{
	HaarWeakClassifier * w1 = new HaarWeakClassifier(*((HaarWeakClassifier*)w));
	return w1;
}

WeakHaarClassifierFactory::~WeakHaarClassifierFactory()
{
}
