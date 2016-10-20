#include <opencv2\opencv.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\ml.hpp>
#include <vector>
#include <iostream>
#include <random>
#include "AdaBoost.h"
#include "VectorData.h"
#include "MatData.h"
#include "WeakVectorClassifierFactory.h"
#include "WeakHaarClassifierFactory.h"

const double PI = 3.14159265;

void test2DPoints()
{
	//Mat img = imread("sky.jpg");
	//Mat gray;
	//cv::cvtColor(img, gray, CV_BGR2GRAY);

	//imshow("Colored Image", img);
	//imshow("Gray Image", gray);

	//waitKey();
	
	// generate the data
	std::vector<DataPoint*> data;
	int i = 0;
	cv::Mat img = cv::Mat::zeros(1100, 1100, CV_32FC3);
	for (double r = 0.1 * 1000; r <= 0.51 * 1000; r += 0.05 * 1000)
	{
		for (double theta = 0; theta <= 1.81 * PI; theta += (PI / 5),i++)
		{
			std::vector<double> d = std::vector<double>(2, 0);
			d[0] = 0.5f + r * std::cos(theta);
			d[1] = 0.5f + (r - 0.05) * std::sin(theta);

			data.push_back(new VectorData(d));
		}
	}
	// test
	//data.clear();
	//label.clear();
	//for (int i = 0; i < 3; i++)
	//{
	//	data.push_back(std::vector<double>(2, 0));
	//	data[i][0] = i;
	//	data[i][1] = rand() % 10;
	//}
	//for (int i = 3; i < 6; i++)
	//{
	//	data.push_back(std::vector<double>(2, 0));
	//	data[i][0] = i;
	//	data[i][1] = rand() % 20;
	//}
	//
	std::cout << data.size() << std::endl;
	int shift = 510;
	std::vector<int> label(30,1); 
	label.resize(data.size(), -1);
	for (int i = 0; i < data.size(); i++)
	{
		cv::circle(img, cv::Point(data[i]->getVectorData()[0] + shift, data[i]->getVectorData()[1] + shift), 
			2, cv::Scalar(((label[i] - 1) / -2) * 255, 0, ((label[i] + 1)/2) * 255), -1);
	}
	//freopen("result.csv", "w", stdout);
	WeakClassifierFactory * factory = new WeakVectorClassifierFactory();
	for (int i = 1; i < 30; i+=2)
	{
		AdaBoost adaboost(i, factory);
		double accuracy = adaboost.train(data, label);
		std::cout << i << ", " << accuracy << std::endl;
		//system("PAUSE");
	}
	cv::imshow("img", img);
	cv::waitKey();
}

void displayIntegralImage(cv::Mat Iimg)
{
	double min, max;
	cv::minMaxLoc(Iimg, &min, &max);
	Iimg -= min;
	Iimg /= (max - min);
	cv::imshow("image", Iimg);
	cv::waitKey();
}

cv::Mat cannyEdgeDetection(cv::Mat m)
{
	/// Canny detector
	cv::Mat detected_edges;
	cv::Canny(m, detected_edges, 10, 30, 3);
	cv::imshow("detected_edges", detected_edges);
	cv::waitKey(50);
	return detected_edges;
}

void testMatData()
{
	cv::Mat m = cv::Mat::zeros(cv::Size(400, 400), CV_8UC1);
	m(cv::Rect(0, 0, 200, 200)) = 50;
	m(cv::Rect(0, 200, 200, 200)) = 100;
	m(cv::Rect(200, 0, 200, 200)) = 200;
	m(cv::Rect(200, 200, 200, 200)) = 70;
	cv::Mat Iimg, I2img;
	cv::integral(m, Iimg, I2img, CV_32FC1);
	// display
	//displayIntegralImage(Iimg);
	cv::Mat edges = cannyEdgeDetection(m);

	// prepare data vector
	std::vector<DataPoint*> trainData;
	std::vector<int> labels;
	cv::Mat show;
	m.copyTo(show);
	// make patches 10x10 with step 5
	for (int r = 0; r <= 390; r += 5)
	{
		for (int c = 0; c <= 390; c += 5)
		{
			cv::Rect win = cv::Rect(r, c, 10, 10);
			trainData.push_back(new MatData(Iimg(win)));
			if (cv::sum(edges(win)).val[0] > 1)
			{
				// edge
				labels.push_back(1);
				cv::rectangle(show, win, cv::Scalar(150, 150, 150));
			}
			else
			{
				// non edge
				labels.push_back(-1);
			}
		}
	}

	cv::imshow("my edges", show);
	cv::waitKey(50);

	// prepare the Haar Classifier Factory
	std::vector<std::vector<std::vector<int> > > shapes;
	int arr[] = { -1,1 };
	shapes.push_back(std::vector<std::vector<int> >(1, std::vector<int>(arr, arr + 2)));
	std::vector<cv::Point> locs(1, cv::Point(0, 0));
	std::vector<cv::Size> sizes(1, cv::Size(5, 10));

	
	for (int i = 1; i < 30; i += 2)
	{
		WeakClassifierFactory * factory = new WeakHaarClassifierFactory(shapes, sizes, locs);

		AdaBoost adaboost(i, factory);
		double accuracy = adaboost.train(trainData, labels);
		std::cout << i << ", " << accuracy << std::endl;
		//system("PAUSE");
	}
}

int main()
{
	//test2DPoints();
	testMatData();
	return 0;
}