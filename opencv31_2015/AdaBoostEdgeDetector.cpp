#include "AdaBoostEdgeDetector.h"

cv::Mat AdaBoostEdgeDetector::cannyEdgeDetection(cv::Mat m)
{
	/// Canny detector
	cv::Mat detected_edges;
	cv::Canny(m, detected_edges, 100, 100, 3);
	//cv::imshow("detected_edges", detected_edges);
	//cv::waitKey(50);
	return detected_edges;
}

AdaBoostEdgeDetector::AdaBoostEdgeDetector(int numClassifiers, std::vector<std::vector<std::vector<int>>>& shapes, cv::Size patchSize, int step)
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
	adaBoost = new AdaBoost(numClassifiers, factory);
}

// assuming images are already grayscale
void AdaBoostEdgeDetector::train(std::vector<cv::Mat> images, bool display)
{
	// prepare data vector
	std::vector<DataPoint*> trainData;
	std::vector<int> labels;
	std::vector<int> imageIndex; // to keep track of images
	for (int i = 0; i < images.size(); i++)
	{
		// integral images
		cv::Mat Iimg, I2img;
		cv::integral(images[i], Iimg, I2img, CV_32FC1);
		cv::Mat edges = AdaBoostEdgeDetector::cannyEdgeDetection(images[i]);
		// make patches 10x10 with step 5
		for (int r = 0; r <= (images[i].rows - patchSize.height); r += step)
		{
			for (int c = 0; c <= (images[i].cols - patchSize.width); c += step)
			{
				cv::Rect win = cv::Rect(c, r, patchSize.width, patchSize.height);
				trainData.push_back(new MatData(Iimg, win));
				imageIndex.push_back(i);
				//cv::Mat testImg = images[i](win);
				//cv::imwrite(std::to_string(i) + "_" + std::to_string(r) + "_" + std::to_string(c) + ".png", testImg);
				//MatData testData(m, win);
				//cv::imshow("window", testData.getMatData());
				//cv::waitKey();
				if (cv::sum(edges(win)).val[0] > 1)
				{
					// edge
					labels.push_back(1);
					//cv::rectangle(show, win, cv::Scalar(255, 0, 0));
				}
				else
				{
					// non edge
					labels.push_back(-1);
				}
			}
		}
	}
	std::vector<int> y;
	double accuracy = adaBoost->train(trainData, labels, y);

	if (display)
	{
		std::vector<cv::Mat> colorImages;
		for (int i = 0; i < images.size(); i++)
		{
			cv::Mat colorImage;
			cv::cvtColor(images[i], colorImage, CV_GRAY2BGR);
			colorImages.push_back(colorImage);
		}
		for (int j = 0; j < y.size(); j++)
		{
			if (y[j] == 1)
			{
				if (labels[j] == 1)
				{
					cv::rectangle(colorImages[imageIndex[j]], ((MatData*)trainData[j])->getROI(), cv::Scalar(0, 255, 0));
				}
				else
				{
					cv::rectangle(colorImages[imageIndex[j]], ((MatData*)trainData[j])->getROI(), cv::Scalar(0, 0, 255));
				}
			}
		}
		for (int i = 0; i < images.size(); i++)
		{
			std::string name = "training_" + std::to_string(numClassifiers) + "_" + std::to_string(i) + ".png";
			cv::imshow(name, colorImages[i]);
			//cv::imwrite(name, colorImages[i]);
		}
		cv::waitKey(0);
	}
	for (int i = 0; i < trainData.size(); i++)
	{
		delete trainData[i];
	}
}

void AdaBoostEdgeDetector::test(cv::Mat img, bool display)
{
	// integral images
	cv::Mat Iimg, I2img;
	cv::integral(img, Iimg, I2img, CV_32FC1);
	cv::Mat edges = AdaBoostEdgeDetector::cannyEdgeDetection(img);
	// make patches 10x10 with step 5
	std::vector<DataPoint*> testData;
	std::vector<cv::Rect> wins;
	std::vector<int> out;
	for (int r = 0; r <= (img.rows - patchSize.height); r += step)
	{
		for (int c = 0; c <= (img.cols - patchSize.width); c += step)
		{
			cv::Rect win = cv::Rect(c, r, patchSize.width, patchSize.height);
			MatData * tmpData = new MatData(Iimg, win);
			out.push_back(adaBoost->classify(tmpData));
			wins.push_back(win);
		}
	}
	if (display)
	{
		cv::Mat colorImage;
		cv::cvtColor(img, colorImage, CV_GRAY2BGR);
		for (int j = 0; j < out.size(); j++)
		{
			if (out[j] == 1)
			{
				cv::circle(colorImage, cv::Point((wins[j].x + wins[j].width / 2), (wins[j].y + wins[j].height / 2)),2,
					cv::Scalar(0, 255, 0), -1);
			}
		}
		std::string name = "testing_" + std::to_string(numClassifiers) + ".png";
		cv::imshow(name, colorImage);
		//cv::imwrite(name, colorImage);
		cv::waitKey(0);
	}
}



AdaBoostEdgeDetector::~AdaBoostEdgeDetector()
{
}
