#include "AdaBoost.h"
#include "VectorData.h"
#include <map>
#include <iostream>
#include <algorithm>

std::vector<WeakClassifier*> AdaBoost::createWeakClassifiersfromVectors(int numClassifiers, std::vector<DataPoint* >& data,bool random)
{
	std::vector<WeakClassifier*> res;
	// sort data in each dimension
	int dims = data[0]->getVectorData().size();
	for (int d = 0; d < dims; d++)
	{
		std::vector<double> dataD(data.size());
		for (int i = 0; i < data.size(); i++)
		{
			dataD[i] = data[i]->getVectorData()[d];
		}
		std::sort(dataD.begin(), dataD.end());
		for (int i = 1; i < dataD.size(); i++)
		{
			double range = dataD[i] - dataD[i - 1];
			int nn = 2;
			for (int j = 0; j < nn; j++)
			{
				double thr = dataD[i - 1] + (range * j) / nn;
				res.push_back(new WeakClassifier(thr, d, 1));
				res.push_back(new WeakClassifier(thr, d, -1));
			}
		}
	}
/*
	// minValues: set the minimum for each data dimension
	// maxValues: set the maximum for each data dimension

	std::vector<double> minValues(data[0]->getVectorData());
	std::vector<double> maxValues(data[0]->getVectorData());

	for (int i = 1; i < data.size(); i++)
	{
		for (int j = 0; j < data[0]->getVectorData().size(); j++)
		{
			minValues[j] = std::min(minValues[j], data[i]->getVectorData()[j]);
			maxValues[j] = std::max(maxValues[j], data[i]->getVectorData()[j]);
		}
	}

	if (random)
	{
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<int> uniformIndex(0, data[0]->getVectorData().size() - 1);
		std::uniform_int_distribution<int> uniformSign(0, 1);
		for (int t = 0; t < numClassifiers; t++)
		{
			int index = uniformIndex(mt);
			int sign = uniformSign(mt) * 2 - 1;
			std::uniform_real_distribution<double> dist(minValues[index], maxValues[index]);
			res.push_back(new WeakClassifier(dist(mt), index, sign));
		}
	}
	else
	{
		// number of classifiers per dimension
		int nPerDim = numClassifiers / data[0]->getVectorData().size();
		for (int i = 0; i < data[0]->getVectorData().size(); i++)
		{
			double diff = (maxValues[i] - minValues[i]) * 2.0 / nPerDim; // two directions
			//std::cout << "diff = " << diff << std::endl;
			for (double thr = minValues[i]; thr < maxValues[i]; thr += diff)
			{
				res.push_back(new WeakClassifier(thr, i, 1));
				res.push_back(new WeakClassifier(thr, i, -1));
			}
		}
	}
	*/
	return res;
}

// numClassifiers: set how many weak classifiers 
// data is vector<vectors> where the outer vector is the data and the second dimesion is the feature vector
AdaBoost::AdaBoost(int numClassifiers)
{
	T = numClassifiers;
}

double AdaBoost::train(std::vector<DataPoint* >& data, std::vector<int>& labels)
{
	std::vector<double> samplesWts(data.size(), 1.0 / labels.size());
	// create T classifers
	for (int t = 0; t < T; t++)
	{
		std::vector<WeakClassifier*> candidates = createWeakClassifiersfromVectors(2000, data, false);
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
		// remove all temp weak classifiers
		for (int i = 0; i < candidates.size(); i++)
		{
			delete candidates[i];
		}
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