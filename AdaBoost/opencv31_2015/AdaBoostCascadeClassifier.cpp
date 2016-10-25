#include "AdaBoostCascadeClassifier.h"
#include "Util.h"

DataPoint * AdaBoostCascadeClassifier::getNormalizedData(cv::Mat Iimg, cv::Mat I2img,cv::Rect& roi)
{
	// normalize to zero mean and unit variance
	double normalizationFactor = 1.0 / (roi.width * roi.height);
	double mean = Util::getRectangleSum(Iimg, roi) * normalizationFactor;
	double xSq = Util::getRectangleSum(I2img, roi);
	double sigmaSquared = xSq  * normalizationFactor - mean * mean;
	double var = std::sqrt(sigmaSquared);

	Iimg /= var;
	return (new MatData(Iimg, roi));
}

// size Factors here must be less than or equal to 1
AdaBoostCascadeClassifier::AdaBoostCascadeClassifier(std::vector<int> numClassifiers, 
	std::vector<std::vector<std::vector<int>>>& shapes, std::vector<double> sizeFactors, cv::Size patchSize, int step)
{
	this->numClassifiers = numClassifiers;
	this->step = step;
	this->patchSize = patchSize;
	std::vector<cv::Size> sizes;
	for (int s = 0; s < sizeFactors.size(); s++)
	{
		for (int i = 0; i < shapes.size(); i++)
		{
			sizes.push_back(cv::Size(sizeFactors[s] * patchSize.width / shapes[i][0].size(), sizeFactors[s] * patchSize.height / shapes[i].size()));
		}
	}
	std::vector<cv::Point> pos;
	for (int r = 0; r < patchSize.height; r+=step)
	{
		for(int c = 0;c < patchSize.width;c+=step)
		{
			pos.push_back(cv::Point(c, r));
		}
	}

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
		cv::Mat Iimg, I2img, tmpImg;
		cv::resize(images[i], tmpImg, patchSize);
		cv::integral(tmpImg, Iimg, I2img, CV_32FC1, CV_32FC1);
		//cv::imshow("tmpImg" + std::to_string(i), tmpImg);
		DataPoint* data = getNormalizedData(Iimg, I2img, cv::Rect(0, 0, patchSize.width, patchSize.height));
		//Util::displayIntegralImage(data->getMatData());
		trainData.push_back(data);
	}
	std::vector<int> y;
	std::vector<double> acc;
	for (int i = 0; i < adaBoostLevels.size(); i++)
	{
		std::cout << "Training Level " << i << std::endl;
		acc.push_back(adaBoostLevels[i]->train(trainData, labels, y, false));
	}
	return y;
}

std::vector<cv::Rect> AdaBoostCascadeClassifier::test(cv::Mat& img, std::vector<double> sizeFactors, bool display)
{
	//cv::imshow("test", img);
	//cv::waitKey();
	std::vector<cv::Rect> res;
	cv::Mat Iimg, I2img;
	cv::integral(img, Iimg, I2img, CV_32FC1, CV_32FC1);
	//Util::displayIntegralImage(Iimg);
	// assume the intial size is the patchSize for now
	for (int s = 0; s < sizeFactors.size(); s++)
	{
		// original window
		cv::Rect win(0, 0, patchSize.width * sizeFactors[s], patchSize.height * sizeFactors[s]);
		// loop over image rows
		for (int r = 0; r <= img.rows - win.height; r+=step)
		{
			win.y = r;
			// loop over image columns
			for (int c = 0; c <= img.cols - win.width; c += step)
			{
				if (display)
				{
					cv::Mat disp = img.clone();
					cv::rectangle(disp, win, cv::Scalar(0, 0, 255), 2);
					cv::imshow("display", disp);
					cv::waitKey(1);
				}
				win.x = c;
				DataPoint * data = getNormalizedData(Iimg, I2img, win);
				//Util::displayIntegralImage(data->getMatData());
				int b = 0;
				for (b = 0; b < adaBoostLevels.size(); b++)
				{
					if (adaBoostLevels[b]->classify(data, sizeFactors[s]) == -1)
					{
						break;
					}
				}
				if (b >= adaBoostLevels.size())
				{
					res.push_back(win);
					std::cout << "Found object at " << win << std::endl;
				}
			}
		}
	}
	
	return res;
}

AdaBoostCascadeClassifier::~AdaBoostCascadeClassifier()
{
}
