#include "AdaBoostCascadeClassifier.h"
#include "Util.h"

DataPoint * AdaBoostCascadeClassifier::getNormalizedData(cv::Mat & Iimg, cv::Mat & I2img,cv::Rect& roi)
{
	// normalize to zero mean and unit variance
	double normalizationFactor = 1.0 / (roi.width * roi.height);
	double mean = Util::getRectangleSum(Iimg, roi) * normalizationFactor;
	double sigmaSquared = mean * mean - Util::getRectangleSum(I2img, roi)  * normalizationFactor;
	double var = std::sqrt(sigmaSquared);

	Iimg /= var;
	return (new MatData(Iimg, roi));
}

AdaBoostCascadeClassifier::AdaBoostCascadeClassifier(std::vector<int> numClassifiers, 
	std::vector<std::vector<std::vector<int>>>& shapes, cv::Size patchSize, int step)
{
	this->numClassifiers = numClassifiers;
	this->step = step;
	this->patchSize = patchSize;
	std::vector<cv::Size> sizes;
	for (int i = 0; i < shapes.size(); i++)
	{
		sizes.push_back(cv::Size(patchSize.width / shapes[i][0].size(), patchSize.height / shapes[i].size()));
	}
	std::vector<cv::Point> pos(1, cv::Point(0, 0));
	factory = new WeakHaarClassifierFactory(shapes, sizes, pos);
	for (int i = 0; i < numClassifiers.size(); i++)
	{
		adaBoostLevels.push_back(new AdaBoost(numClassifiers[i], factory));
	}
	
}

std::vector<int> AdaBoostCascadeClassifier::train(std::vector<cv::Mat>& images, std::vector<int>& labels)
{
	// prepare data vector
	std::vector<DataPoint*> trainData;
	for (int i = 0; i < images.size(); i++)
	{
		cv::Mat Iimg, I2img;
		cv::integral(images[i], Iimg, I2img, CV_32FC1);
		trainData.push_back(getNormalizedData(Iimg, I2img, cv::Rect(0,0, patchSize.width, patchSize.height)));
	}
	std::vector<int> y;
	std::vector<double> acc;
	for (int i = 0; i < adaBoostLevels.size(); i++)
	{
		acc.push_back(adaBoostLevels[i]->train(trainData, labels, y));
	}
	return y;
}

std::vector<cv::Rect> AdaBoostCascadeClassifier::test(cv::Mat& img, std::vector<double> sizeFactors)
{
	std::vector<cv::Rect> res;
	cv::Mat Iimg, I2img;
	cv::integral(img, Iimg, I2img, CV_32FC1);
	// assume the intial size is the patchSize for now
	for (int s = 0; s < sizeFactors.size(); s++)
	{
		// original window
		cv::Rect win(0, 0, patchSize.width * sizeFactors[s], patchSize.height * sizeFactors[s]);
		// loop over image rows
		for (int r = 0; r < img.rows - win.height; r+=step)
		{
			// loop over image columns
			for (int c = 0; c < img.cols - win.width; c += step)
			{
				DataPoint * data = getNormalizedData(Iimg, I2img, win);
				int b = 0;
				for (int b = 0; b < adaBoostLevels.size(); b++)
				{
					if (adaBoostLevels[b]->classify(data, sizeFactors[s]) == -1)
					{
						break;
					}
				}
				if (b >= adaBoostLevels.size())
				{
					res.push_back(((MatData*)data)->getROI());
				}
			}
		}
	}
	
	return res;
}

AdaBoostCascadeClassifier::~AdaBoostCascadeClassifier()
{
}
