#pragma once
#include "WeakClassifierFactory.h"
#include <map>
class WeakHaarClassifierFactory :
	public WeakClassifierFactory
{
protected:
	std::vector<std::vector<std::vector<int> > > shapes;
	std::vector<cv::Size> sizes;
	std::vector<cv::Point> positions;
	std::map<double, int> flags;
public:
	WeakHaarClassifierFactory(std::vector<std::vector<std::vector<int> > > & shapes, std::vector<cv::Size> & sizes, std::vector<cv::Point>& positions);
	virtual std::vector<WeakClassifier*> getClassifiers(DataPoint*);
	virtual WeakClassifier* copyClassifier(WeakClassifier* w);
	~WeakHaarClassifierFactory();
};

