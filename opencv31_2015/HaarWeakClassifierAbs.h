#pragma once
#include "HaarWeakClassifier.h"
#include <cmath>
#include <cstdlib>
class HaarWeakClassifierAbs :
	public HaarWeakClassifier
{
	int dummy;
public:
	HaarWeakClassifierAbs(cv::Size, cv::Point, std::vector<std::vector<int> >, int direction);
	virtual int classify(DataPoint * d);
	virtual bool operator==(WeakClassifier *obj);
	virtual void operator=(const WeakClassifier& obj);
};

