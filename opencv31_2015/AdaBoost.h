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
	std::vector<WeakClassifier*> createWeakClassifiers(std::vector<DataPoint* >&);
public:
	// numClassifiers: set how many weak classifiers 
	// data is vector<vectors> where the outer vector is the data and the second dimesion is the feature vector
	AdaBoost(int numClassifiers, WeakClassifierFactory*);
	// return the acccuray
	double train(std::vector<DataPoint* >& data, std::vector<int>& labels);
	// return class 1 or -1
	int classify(DataPoint* subj);
	~AdaBoost();
};