#include "WeakClassifier.h"
#include "VectorData.h"
#include <vector>




void WeakClassifier::updateEdgewithClassification(DataPoint* d)
{
	classify(d);
	setEdge(getData());
}

WeakClassifier::WeakClassifier(double edge, int index, int direction)
{
	this->edge = edge;
	this->featureIndex = index;
	this->sign = direction;
}


int WeakClassifier::classify()
{
	if (sign * (data - edge) < 0)
	{
		return 1;
	}
	return -1;
}

int WeakClassifier::classify(DataPoint* d)
{
	data = d->getVectorData()[featureIndex];
	return classify();
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
