#include "WeakClassifier.h"
#include "VectorData.h"
#include <vector>


WeakClassifier::WeakClassifier(double edge, int index, int direction)
{
	this->edge = edge;
	this->featureIndex = index;
	this->sign = direction;
}

int WeakClassifier::classify(DataPoint* d)
{
	std::vector<double> data = d->getVectorData();
	if (sign * (data[featureIndex] - edge) < 0)
	{
		return 1;
	}
	return -1;
}

bool WeakClassifier::operator==(const WeakClassifier & obj)
{
	if (sign != obj.sign)
	{
		return false;
	}
	if (edge - obj.edge > 0.000001)
	{
		return false;
	}
	if (featureIndex != obj.featureIndex)
	{
		return false;
	}
	return true;
}

WeakClassifier & WeakClassifier::operator=(const WeakClassifier & obj)
{
	// TODO: insert return statement here
	sign = obj.sign;
	edge = obj.edge;
	featureIndex = obj.featureIndex;
	
	return *this;
}
