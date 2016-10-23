#pragma once
#include "WeakClassifierFactory.h"
class WeakVectorClassifierFactory :
	public WeakClassifierFactory
{
public:
	virtual std::vector<WeakClassifier*> getClassifiers(DataPoint* d);
	virtual WeakClassifier* copyClassifier(WeakClassifier* w);
};

