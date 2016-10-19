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
	double data;
	int classify();
public:
	void setEdge(double e) { edge = e; }
	void setFeatureIndex(int i) { featureIndex = 1; }
	void setSign(int s) { sign = s; }
	double getData() { return data; }
	void updateEdgewithClassification(DataPoint* d);
	WeakClassifier(double edge,int index,int direction);
	virtual int classify(DataPoint* d);
	virtual bool operator==(const WeakClassifier &obj);
	virtual WeakClassifier& operator=(const WeakClassifier &obj);
};

