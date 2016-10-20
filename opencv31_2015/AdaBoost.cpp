#include "AdaBoost.h"
#include <map>
#include <iostream>
#include <algorithm>

std::vector<WeakClassifier*> AdaBoost::createWeakClassifiers(const std::vector<DataPoint* >& data)
{
	std::vector<WeakClassifier*> res;
	for (int i = 0; i < data.size(); i++)
	{
		std::vector<WeakClassifier*> tmp = weakClassifierFactory->getClassifiers(data[i]);
		for (int j = 0; j < tmp.size(); j++)
		{
			int k = 0;
			for (; k < res.size(); k++)
			{
				if (tmp[j]->operator==(res[k]))
				{
					break;
				}
			}
			if (k >= res.size())
			{
				res.push_back(tmp[j]);
			}
		}
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

double AdaBoost::train(const std::vector<DataPoint* >& data,const std::vector<int>& labels, std::vector<int>& yOut)
{
	std::vector<double> samplesWts(data.size(), 1.0 / labels.size());
	
	int counts[2] = { 0,0 };
	for (int i = 0; i < labels.size(); i++)
	{
		counts[(labels[i] + 1) / 2]++;
	}
	for (int i = 0; i < samplesWts.size(); i++)
	{
		if (labels[i] == 1)
		{
			samplesWts[i] = 1.0 / (2.0 * counts[1]);
		}
		else
		{
			samplesWts[i] = 1.0 / (2.0 * counts[0]);
		}
	}
	double sss[2] = { 0 };
	for (int i = 0; i < samplesWts.size(); i++)
	{
		sss[(labels[i] + 1) / 2] += samplesWts[i];
	}
	// create T classifers
	std::vector<WeakClassifier*> candidates = createWeakClassifiers(data);

	for (int t = 0; t < T; t++)
	{
		std::multimap<double, WeakClassifier*> classifierAccuracyMap;
		// get the accuracy of each weak classifier
#pragma omp parallel for shared(classifierAccuracyMap)
		for (int i = 0; i < candidates.size(); i++)
		{
			double errors = 0;
			//std::vector<int> y;
			for (int j = 0; j < data.size(); j++)
			{
				int res = candidates[i]->classify(data[j]);
				//y.push_back(res);
				if (res != labels[j])
				{
					errors += samplesWts[j];
				}
			}
#pragma omp critical 
			classifierAccuracyMap.insert(std::make_pair(errors, candidates[i]));
		}

		// make sure the classifiers are unique
		

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
				//samplesWts[k] = samplesWts[k] * std::pow(itr->first / (1 - itr->first), 1 - (labels[k] ^ itr->second->classify(data[k])));
				sumSamplesWts += samplesWts[k];
			}
			// and normalize the weights
			for (int k = 0; k < labels.size(); k++)
			{
				samplesWts[k] /= sumSamplesWts;
			}
			std::cout << t << ":" << wts[t] << "\t";
			itr->second->print();
			std::cout << std::endl;
			// finally add the classifier
			weakClassifiers.push_back(weakClassifierFactory->copyClassifier(itr->second));
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
		//if (i == 21)
		//{
		//	std::cout << "break";
		//}

		yOut.push_back(classify(data[i]));
		if (yOut[i] == labels[i])
		{
			acc++;
			if (labels[i] == 1)
			{
				classify(data[i]);
				//std::cout << "edge at index " << i << std::endl;
			}
		}

	}
	//std::cout << acc << std::endl;
	double accuracy = acc;
	return accuracy / labels.size();
}
int AdaBoost::classify(DataPoint* subj)
{
	std::vector<double> r(weakClassifiers.size(),0);
#pragma omp parallel for
	for (int i = 0; i < weakClassifiers.size(); i++)
	{
		r[i] = weakClassifiers[i]->classify(subj) * wts[i];
	}
	double res = 0;
	for (int i = 0; i < r.size(); i++)
	{
		res += r[i];
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