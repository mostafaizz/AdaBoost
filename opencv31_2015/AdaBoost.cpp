#include "AdaBoost.h"
#include <map>
#include <iostream>
#include <algorithm>

std::vector<WeakClassifier*> AdaBoost::createWeakClassifiers(std::vector<DataPoint* >& data)
{
	std::vector<WeakClassifier*> res;
	for (int i = 0; i < data.size(); i++)
	{
		std::vector<WeakClassifier*> tmp = weakClassifierFactory->getClassifiers(data[i]);
		res.insert(res.end(), tmp.begin(), tmp.end());
	}
	return res;
}

// numClassifiers: set how many weak classifiers 
// data is vector<vectors> where the outer vector is the data and the second dimesion is the feature vector
AdaBoost::AdaBoost(int numClassifiers, WeakClassifierFactory* weakClassifierFactory)
{
	T = numClassifiers;
	this->weakClassifierFactory = weakClassifierFactory;
}

double AdaBoost::train(std::vector<DataPoint* >& data, std::vector<int>& labels)
{
	std::vector<double> samplesWts(data.size(), 1.0 / labels.size());
	/*
	int counts[2] = { 0,0 };
	for (int i = 0; i < labels.size(); i++)
	{
		counts[(labels[i] + 1) / 2]++;
	}
	for (int i = 0; i < samplesWts.size(); i++)
	{
		if (labels[i] == -1)
		{
			samplesWts[i] = 1.0 / (2.0 * counts[1]);
		}
		else
		{
			samplesWts[i] = 1.0 / (2.0 * counts[0]);
		}
	}*/
	// create T classifers
	std::vector<WeakClassifier*> candidates = createWeakClassifiers(data);

	for (int t = 0; t < T; t++)
	{
		std::multimap<double, WeakClassifier*> classifierAccuracyMap;
		// get the accuracy of each weak classifier
		for (int i = 0; i < candidates.size(); i++)
		{
			double errors = 0;
			std::vector<int> y;
			for (int j = 0; j < data.size(); j++)
			{
				int res = candidates[i]->classify(data[j]);
				y.push_back(res);
				if (res != labels[j])
				{
					errors += samplesWts[j];
				}
			}
			classifierAccuracyMap.insert(std::make_pair(errors, candidates[i]));
		}

		for (std::multimap<double, WeakClassifier*>::iterator itr = classifierAccuracyMap.begin();
			itr != classifierAccuracyMap.end() && weakClassifiers.size() != T; itr++)
		{
			// break if the classifier has error less than half
			if (itr->first >= 0.5)
			{
				break;
			}
			// check if the classifier is already used
			/*int j = 0;
			for (; j < weakClassifiers.size(); j++)
			{
				if (*(itr->second) == *weakClassifiers[j])
				{
					break;
				}
			}
			if (j < weakClassifiers.size())
			{
				// contiunue if the classifier is already used
				continue;
			}*/
			// if it is okay to use this classifer, we need to find the weight for this classifier
			// add the classifer weight
			double alpha = 0;
			if (itr->first < 0.000001)
			{
				alpha = 1000000;
				T = weakClassifiers.size() + 1;
			}
			else
			{
				alpha = 0.5 * std::log((1 - itr->first) / itr->first);
			}
			wts.push_back(alpha);
			// update label weights
			double sumSamplesWts = 0;
			for (int k = 0; k < labels.size(); k++)
			{
				samplesWts[k] = samplesWts[k] * std::exp(-alpha * labels[k] * itr->second->classify(data[k]));
				sumSamplesWts += samplesWts[k];
			}
			// and normalize the weights
			for (int k = 0; k < labels.size(); k++)
			{
				samplesWts[k] /= sumSamplesWts;
			}
			//std::cout << itr->first << " " << itr->second->edge << " " << itr->second->featureIndex << " " << itr->second->sign << std::endl;
			// finally add the classifier
			weakClassifiers.push_back(new WeakClassifier(*itr->second));
			// then break this loop
			break;
		}
	}
	// remove all temp weak classifiers
	for (int i = 0; i < candidates.size(); i++)
	{
		delete candidates[i];
	}
	// calculate and return the accuracy
	int acc = 0;
	for (int i = 0; i < labels.size(); i++)
	{
		if (classify(data[i]) == labels[i])
		{
			acc++;
		}
	}
	//std::cout << acc << std::endl;
	double accuracy = acc;
	return accuracy / labels.size();
}
int AdaBoost::classify(DataPoint* subj)
{
	double res = 0;
	for (int i = 0; i < weakClassifiers.size(); i++)
	{
		res += weakClassifiers[i]->classify(subj) * wts[i];
	}
	if (res >= 0)
	{
		return 1;
	}
	else if (res < 0)
	{
		return -1;
	}
	return 0;
}
AdaBoost::~AdaBoost()
{
	for (int i = 0; i < weakClassifiers.size(); i++)
	{
		delete weakClassifiers[i];
	}
}