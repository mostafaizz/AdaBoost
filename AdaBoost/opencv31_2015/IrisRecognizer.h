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
	cv::Mat grayImg, OrigImg, irisStrip;
	double xPupil, yPupil, pupilRdaius;
	cv::Size stripSize;
	std::vector<cv::Mat> filtersBank;
public:
	IrisRecognizer(cv::Size stripSize)
	{
		this->stripSize = stripSize;
		
		//float freq = 0.6;
		//for (int cnt = 0; cnt < 8; cnt++, freq *= std::sqrt(2.0))
		for(float freq = 0.6; freq > 0.05;freq /= std::sqrt(2.0))
		{
			for (double theta = 0; theta < CV_PI; theta += CV_PI / 8.0)
			{
				//std::cout << theta << std::endl;
				filtersBank.push_back(cv::getGaborKernel(stripSize, stripSize.width, theta, 1/freq, 
					stripSize.height * 1.0 / stripSize.width));
				//cv::imshow("Filter 1 ", filtersBank[filtersBank.size() - 1]);

				//double min, max;
				//cv::minMaxLoc(filtersBank[filtersBank.size() - 1], &min, &max);
				//std::cout << "Min = " << min << "\tMax = " << max << std::endl;
				//cv::Mat test;
				//cv::filter2D(floatStrip, test, CV_32FC1, filtersBank[filtersBank.size() - 1]);
				//cv::imshow("test", test);
				filtersBank.push_back(cv::getGaborKernel(stripSize, stripSize.width, theta, 1 / freq, 
					stripSize.height * 1.0 / stripSize.width, 0));
				//cv::imshow("Filter 2 ", filtersBank[filtersBank.size() - 1]);
				//cv::waitKey();

			}
		}
	}
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

	// return the ratio between the estimated circle circumferance and the number of pixels in the contour
	double isCircle(const cv::Mat& compnent, double& xC, double& yC, double& radius)
	{
		std::vector<cv::Point> hull;
		cv::convexHull(compnent, hull);

		xC = 0, yC = 0;
		for (int r = 0; r < hull.size(); r++)
		{
			xC += hull[r].x;
			yC += hull[r].y;
		}
		xC /= hull.size();
		yC /= hull.size();

		// get the radius as the median radius
		std::vector<double> radiusSquared;
		radius = 0;
		for (int r = 0; r < hull.size(); r++)
		{
			double a = xC - hull[r].x;
			double b = yC - hull[r].y;

			radiusSquared.push_back(a * a + b * b);

			//radius += std::sqrt(radiusSquared[r]);
		}


		//radius = (radius / radiusSquared.size());

		std::nth_element(radiusSquared.begin(), radiusSquared.begin() + radiusSquared.size() / 2, radiusSquared.end());
		radius = std::sqrt(radiusSquared[radiusSquared.size() / 2]);
		//radius = std::sqrt(*(std::max_element(radiusSquared.begin(), radiusSquared.end())));
		std::vector<cv::Point2f> contour;
		cv::approxPolyDP(cv::Mat(hull), contour, 0.001, true);
		double area = std::fabs(cv::contourArea(cv::Mat(contour)));
		double circumfrance = (2 * 3.14 * radius);
		double test = (3.14 * radius * radius) / area;

		std::cout << "Component Circle Test = " << test << std::endl;

		return std::abs(test - 1);
	}

	// input is the original gray scale image and the ouptut is the mask for the pupil
	void extractPupil() {

		for (int i = 0; i < 20; i++)
		{
			cv::medianBlur(grayImg, grayImg, 7);
		}

		cv::Mat res, hist;
		/// Establish the number of bins
		int histSize = 64;

		/// Set the ranges ( for B,G,R) )
		float range[] = { 0, 256 };
		const float* histRange = { range };

		bool uniform = true; bool accumulate = false;

		/// Compute the histograms:
		//std::cout << grayImg.channels() << std::endl;
		cv::calcHist(&grayImg, 1, 0, cv::Mat(), res, 1, &histSize, &histRange, uniform, accumulate);

		std::vector<double> data, firstD, secondD;

		for (int r = 0; r < res.rows; r++)
		{
			data.push_back(res.at<int>(cv::Point(0, r)));
		}
		for (int r = 1; r < data.size(); r++)
		{
			firstD.push_back(data[r] - data[r - 1]);
			//std::cout << (r - 1) *  256 / histSize << "\t" << firstD[r - 1] << std::endl;
		}

		int thresh = 50;
		for (int i = 1; i < firstD.size(); i++)
		{
			if (firstD[i] < 0 && firstD[i - 1] > 0)
			{
				thresh = (i + 2) * 256 / histSize;
				break;
			}
		}
		
		std::cout << "thresh = " << thresh << std::endl;

		cv::threshold(grayImg, res, thresh, 255, cv::THRESH_BINARY_INV);

		//cv::medianBlur(res, res, 7);
		
		/*
		for (int counter = 0; counter < 10; counter++)
		{
			cv::erode(res, res, cv::Mat());
		}
		for (int counter = 0; counter < 3; counter++)
		{
			cv::dilate(res, res, cv::Mat());
		}
		*/
		//grayImg.copyTo(grayImg, res);
		//cv::imshow("pupil mask", res);
		
		std::vector<cv::Mat> connectedComponenets;
		cv::findContours(res, connectedComponenets, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		std::cout << "Components found = " << connectedComponenets.size() << std::endl;
		
		// get the most reasonable circle 
		double testVAlue = 100;
		double xC = 0, yC = 0, radius = 0;
		for (int i = 0; i < connectedComponenets.size(); i++)
		{
			double x = 0, y = 0, r = 0;
			double test = isCircle(connectedComponenets[i], x, y, r);
			if (test/r < testVAlue)
			{
				testVAlue = test/r;
				radius = r;
				xC = x;
				yC = y;
			}
		}
		
		xPupil = xC;
		yPupil = yC;
		pupilRdaius = radius;

		std::cout << "Pupil Radius = " << radius << std::endl;
		
		//cv::circle(OrigImg, cv::Point(xC, yC), radius, cv::Scalar(255, 0, 0));
		//cv::imshow("pupil", OrigImg);
		//cv::waitKey(0);

		//return res;
	}
	cv::Mat getOrigImage()
	{
		return OrigImg;
	}
	cv::Mat extractIris()
	{
		/// Reduce the noise so we avoid false circle detection
		cv::GaussianBlur(grayImg, grayImg, cv::Size(9, 9), 2, 2);
		cv::Mat detected_edges;
		
		cv::Canny(grayImg, detected_edges, 2, 6);
		std::map<int, std::map<int, int> > radMap;
		
		// hough transform from the pupil center point
		for (int r = 0; r < detected_edges.rows; r++)
		{
			for (int c = 0; c < detected_edges.cols; c++)
			{
				if (detected_edges.at<unsigned char>(cv::Point(c, r)))
				{
					// edge
					double radiuRatio = 0.1;
					for (int x = xPupil - radiuRatio * pupilRdaius; 
						x <= xPupil + radiuRatio * pupilRdaius; 
						x += 0.1 * pupilRdaius)
					{
						for (int y = yPupil - radiuRatio * pupilRdaius; 
							y <= yPupil + radiuRatio * pupilRdaius; 
							y += 0.1 * pupilRdaius)
						{
							int rad = std::sqrt((c - x) * (c - x) + (r - y) * (r - y));
							if (rad > pupilRdaius*1.5 && rad <= pupilRdaius * 4)
							{
								int pt = x + grayImg.cols * y;
								if (radMap.find(pt) == radMap.end())
								{
									radMap[pt] = std::map<int, int>();
								}
								radMap[pt][rad] = radMap[pt][rad] + 1;
							}
						}
					}
					
				}
			}
		}
		double irisRadius, bestRatio = 100;
		int irisX, irisY;
		for (std::map<int, std::map<int, int> >::iterator itr = radMap.begin(); itr != radMap.end(); itr++)
		{
			// sorting
			std::map<double, int> radMapSorted;
			for (std::map<int, int>::iterator itr1 = (itr->second).begin(); itr1 != (itr->second).end(); itr1++)
			{
				double circ = 2 * itr1->first * 3.14;
				double ratio = circ / itr1->second; // 1.0 / itr->second;
				radMapSorted[ratio] = itr1->first;
			}
			if (radMapSorted.begin()->first < bestRatio)
			{
				irisX = itr->first % grayImg.cols;
				irisY = itr->first / grayImg.cols;

				bestRatio = radMapSorted.begin()->first;
				irisRadius = radMapSorted.begin()->second;
			}
		}

		cv::Mat strip;
		cv::linearPolar(OrigImg, strip, cv::Point(xPupil, yPupil), irisRadius, cv::INTER_CUBIC);
		cv::resize(strip, strip, cv::Size(irisRadius, 2 * 3.14 * irisRadius));
		cv::Rect roi(pupilRdaius, 0, strip.cols - pupilRdaius, strip.rows);
		strip = strip(roi).t();
		// normalize r=[0,64], Theta=[0,360]
		cv::resize(strip, strip, stripSize);

		irisStrip = strip.clone();
		std::cout << irisStrip.channels() << std::endl;
		std::cout << "Iris Radius = " << irisRadius << std::endl;
		cv::circle(OrigImg, cv::Point(xPupil, yPupil), pupilRdaius, cv::Scalar(255, 0, 0), 2);
		cv::circle(OrigImg, cv::Point(xPupil, yPupil), irisRadius, cv::Scalar(0, 0, 255), 2);
		
		//cv::dilate(detected_edges, detected_edges, cv::Mat());
		//for (int i = 0; i < 10; i++)
		//{
		//	cv::medianBlur(detected_edges, detected_edges, 7);
		//}

		

		//cv::imshow("strip", strip);
		//cv::imshow("gray", grayImg);
		//cv::imshow("detected_edges", detected_edges);
		//cv::imshow("pupil and iris", OrigImg);
		//cv::waitKey();
		// hough transform

		//return res;
		return strip;
	}


	cv::Mat calcFeatures()
	{
		// convert the image to float and normalize to [0,1)
		cv::Mat floatStrip;
		cv::cvtColor(irisStrip, floatStrip, cv::COLOR_BGR2GRAY);
		floatStrip.convertTo(floatStrip, CV_32F);
		
		cv::normalize(floatStrip, floatStrip,1,0, cv::NORM_MINMAX);
		//floatStrip = (floatStrip * 2) - 1;
		

		cv::Mat real = cv::Mat::zeros(stripSize, CV_32F), imag = cv::Mat::zeros(stripSize, CV_32F);
		for (int i = 0; i < filtersBank.size(); i += 2)
		{
			cv::Mat test1, test2;
			cv::filter2D(floatStrip, test1, CV_32F, filtersBank[i]);
			real = real + test1;
			cv::filter2D(floatStrip, test2, CV_32F, filtersBank[i + 1]);
			imag = imag + test2;
			/*
			cv::normalize(test1, test1, 1, 0, cv::NORM_MINMAX);
			cv::normalize(test2, test2, 1, 0, cv::NORM_MINMAX);
			cv::imshow("test1", test1);
			cv::imshow("test2", test2);
			cv::Mat real1,imag1;
			cv::normalize(real, real1, 1, 0, cv::NORM_MINMAX);
			cv::normalize(imag, imag1, 1, 0, cv::NORM_MINMAX);
			cv::imshow("real", real1);
			cv::imshow("imag", imag1);
			cv::imshow("floatStrip", floatStrip);
			cv::waitKey();
			*/
		}

		//double min, max;
		//cv::minMaxLoc(imag, &min, &max);
		//std::cout << "Min = " << min << "\tMax = " << max << std::endl;
		cv::normalize(real, real, 1, 0, cv::NORM_MINMAX);
		cv::normalize(imag, imag, 1, 0, cv::NORM_MINMAX);
		std::vector<cv::Mat> parts(2, cv::Mat());
		cv::threshold(real, parts[0], 0.5, 255, cv::THRESH_BINARY);
		parts[0].convertTo(parts[0], CV_8UC1);
		cv::threshold(imag, parts[1], 0.5, 255, cv::THRESH_BINARY);
		parts[1].convertTo(parts[1], CV_8UC1);

		std::cout << parts[0].channels() << std::endl;

		std::vector<cv::Mat> code;
		for (int i = 0; i < 3; i++)
		{
			code.push_back(cv::Mat::zeros(stripSize.height, stripSize.width * 2, CV_8UC1));
		}
		for (int c = 0; c < stripSize.width; c++)
		{
			parts[0].col(c).copyTo(code[0].col(c * 2));
			parts[1].col(c).copyTo(code[0].col(c * 2 + 1));
		}
		cv::Mat code1;
		cv::merge(code, code1);
		std::cout << code1.channels() << std::endl;
		//cv::hconcat(parts, code);

		//cv::imshow("parts[0]", parts[0]);
		//cv::imshow("parts[1]", parts[1]);
		//cv::waitKey();
		return code[0];
	}
};