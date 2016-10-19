#include <opencv2\opencv.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\ml.hpp>
#include <vector>
#include <iostream>
#include <random>
#include "AdaBoost.h"
#include "VectorData.h"
#include "WeakVectorClassifierFactory.h"

const double PI = 3.14159265;

int main()
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
	return 0;
}