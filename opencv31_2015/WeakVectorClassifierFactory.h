#pragma once
#include "WeakClassifierFactory.h"
class WeakVectorClassifierFactory :
	public WeakClassifierFactory
{
public:
	virtual std::vector<WeakClassifier*> getClassifiers(DataPoint* d);
};

