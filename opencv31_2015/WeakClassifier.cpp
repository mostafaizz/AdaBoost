#include "WeakClassifier.h"
#include "VectorData.h"
#include <vector>



// return teh classification result to know if there is a problem (when 0 is returned)
int WeakClassifier::updateEdgewithClassification(DataPoint* d)
{
	int res = classify(d);
	setEdge(getData());
	return res;
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

bool WeakClassifier::operator==(WeakClassifier * obj)
{
	if (sign != obj->sign)
	{
		return false;
	}
	if (std::abs(edge - obj->edge) > 0.000001)
	{
		return false;
	}
	if (featureIndex != obj->featureIndex)
	{
		return false;
	}
	return true;
}

void WeakClassifier::operator=(const WeakClassifier & obj)
{
	// TODO: insert return statement here
	sign = obj.sign;
	edge = obj.edge;
	featureIndex = obj.featureIndex;
}
