#pragma once
#include <vector>
#include <random>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include "WeakClassifier.h"
#include "WeakClassifierFactory.h"

class AdaBoost
{
private:
	int T; // number of weak classifiers
	WeakClassifierFactory * weakClassifierFactory;
	std::vector<WeakClassifier*> weakClassifiers;
	std::vector<double> wts;
	std::vector<WeakClassifier*> createWeakClassifiers(const std::vector<DataPoint* >&, bool unique);
public:
	// numClassifiers: set how many weak classifiers 
	// data is vector<vectors> where the outer vector is the data and the second dimesion is the feature vector
	AdaBoost(int numClassifiers, WeakClassifierFactory*);
	// return the acccuray
	double train(const std::vector<DataPoint* >& data,const std::vector<int>& labels, std::vector<int>& y,bool unique);
	// return class 1 or -1
	int classify(DataPoint* subj, double sizeFactor);
	~AdaBoost();
};