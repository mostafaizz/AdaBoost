#include "WeakVectorClassifierFactory.h"

std::vector<WeakClassifier*> WeakVectorClassifierFactory::getClassifiers(DataPoint * d)
{
	std::vector<WeakClassifier*> res;
	int dims = d->getDimensions();
	for (int i = 0; i < dims;i++)
	{
		WeakClassifier * w = new WeakClassifier(0, i, 1);
		w->updateEdgewithClassification(d);
		res.push_back(w);

		WeakClassifier * w1 = new WeakClassifier(0, i, -1);
		w1->updateEdgewithClassification(d);
		res.push_back(w1);
	}
	return res;
}
