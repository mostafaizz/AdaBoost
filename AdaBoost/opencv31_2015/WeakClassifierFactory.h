#pragma once
#include "WeakClassifier.h"
#include "DataPoint.h"
#include <vector>
class WeakClassifierFactory
{
public:
	virtual std::vector<WeakClassifier*> getClassifiers(DataPoint*) = 0;
	virtual WeakClassifier* copyClassifier(WeakClassifier*) = 0;
};

