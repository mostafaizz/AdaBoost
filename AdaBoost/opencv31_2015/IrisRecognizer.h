#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>


class IrisRecognizer
{
private:
	cv::Mat grayImg, OrigImg;
public:
	void IrisRecognizerRead(cv::Mat inputImg) {
		OrigImg = inputImg.clone();
		if (OrigImg.channels() == 3) {
			cv::cvtColor(OrigImg, grayImg, cv::COLOR_BGR2GRAY);
		}
		else
		{
			grayImg = inputImg.clone();
		}
	}
	void IrisRecognizerRead(char* inputImgName) {
		OrigImg = cv::imread(inputImgName);
		if (OrigImg.channels() == 3) {
			cv::cvtColor(OrigImg, grayImg, cv::COLOR_BGR2GRAY);
		}
		else
		{
			grayImg = OrigImg.clone();
		}
	}
	void showCurrentEyeImage()
	{
		cv::imshow("Eye Image", OrigImg);
		cv::waitKey();
	}
	// input is the original gray scale image and the ouptut is the mask for the pupil
	cv::Mat extractPupil() {
		cv::Mat res, hist;
		/// Establish the number of bins
		//int histSize = 2;

		/// Set the ranges ( for B,G,R) )
		//float range[] = { 0, 256 };
		//const float* histRange = { range };

		//bool uniform = true; bool accumulate = false;

		/// Compute the histograms:
		//std::cout << grayImg.channels() << std::endl;
		//cv::calcHist(&grayImg, 1, 0, cv::Mat(), res, 1, &histSize, &histRange, uniform, accumulate);
		int thresh = 50;
		
		cv::threshold(grayImg, res, thresh, 255, cv::THRESH_BINARY_INV);

		medianBlur(res, res, 7);

		/*
		for (int counter = 0; counter < 10; counter++)
		{
			cv::erode(res, res, cv::Mat());
		}*/
		for (int counter = 0; counter < 10; counter++)
		{
			cv::dilate(res, res, cv::Mat());
		}
		
		std::vector<cv::Mat> connectedComponenets;
		cv::findContours(res, connectedComponenets, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		std::cout << connectedComponenets.size() << std::endl;
		int maxComp = 0;
		for (int i = 1; i < connectedComponenets.size(); i++)
		{
			if (connectedComponenets[i].rows > connectedComponenets[maxComp].rows)
			{
				maxComp = i;
			}
		}
		// check if the largest component is a circle
		double xC = 0, yC = 0;
		for (int r = 0; r < connectedComponenets[maxComp].rows; r++)
		{
			xC += connectedComponenets[maxComp].at<int>(cv::Point(0, r));
			yC += connectedComponenets[maxComp].at<int>(cv::Point(1, r));
		}
		xC /= connectedComponenets[maxComp].rows;
		yC /= connectedComponenets[maxComp].rows;

		// get the radius as the median radius
		std::vector<double> radiusSquared;
		for (int r = 0; r < connectedComponenets[maxComp].rows; r++)
		{
			double a = xC  - connectedComponenets[maxComp].at<int>(cv::Point(0, r));
			double b = yC  - connectedComponenets[maxComp].at<int>(cv::Point(1, r));

			radiusSquared.push_back(a * a + b * b);
		}
		std::vector<double>::iterator itr;
		std::nth_element(radiusSquared.begin(), radiusSquared.begin() + radiusSquared.size()/2, radiusSquared.end());

		double radius = std::sqrt(radiusSquared[radiusSquared.size() / 2]);

		std::cout << radius << std::endl;
		
		cv::circle(OrigImg, cv::Point(xC, yC), radius, cv::Scalar(255, 0, 0));
		cv::imshow("pupil", OrigImg);
		cv::waitKey();

		return res;
	}
	cv::Mat extractIris(cv::Mat pupilMask)
	{
		cv::Mat res;

		return res;
	}

};