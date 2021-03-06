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
    void setFeatureIndex(int i) { featureIndex = i; }
	void setSign(int s) { sign = s; }
	double getData() { return data; }
	int updateEdgewithClassification(DataPoint* d);
	WeakClassifier(double edge,int index,int direction);
	virtual int classify(DataPoint* d, double sizeFactor);
	virtual bool operator==(WeakClassifier *obj);
	virtual void operator=(const WeakClassifier &obj);
	virtual std::string getName();
	virtual void print();
};

