#pragma once
#include "WeakClassifierFactory.h"
class WeakHaarClassifierFactory :
	public WeakClassifierFactory
{
protected:
	std::vector<std::vector<std::vector<int> > > shapes;
	std::vector<cv::Size> sizes;
	std::vector<cv::Point> positions;
public:
	WeakHaarClassifierFactory(std::vector<std::vector<std::vector<int> > > & shapes, std::vector<cv::Size> & sizes, std::vector<cv::Point>& positions);
	virtual std::vector<WeakClassifier*> getClassifiers(DataPoint*);
	~WeakHaarClassifierFactory();
};

