#pragma once
#include "DataPoint.h"

///
// The class implement simple WeakClassifer based on one of the dimensions and using a cutting point to give classification
// the equation should be (sign * data[featureIndex]) >= edge 
class WeakClassifier
{
protected:
	double edge;
	int featureIndex;
	int sign;
public:
	WeakClassifier(double edge,int index,int direction);
	virtual int classify(DataPoint* data);
	virtual bool operator==(const WeakClassifier &obj);
	virtual WeakClassifier& operator=(const WeakClassifier &obj);
};

