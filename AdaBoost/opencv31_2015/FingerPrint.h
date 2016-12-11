#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include "thining.h"


class FingerPrintRecognizer
{
public:
	void run(const char* fileName)
	{
		// read gray image
		cv::Mat fOrig = cv::imread(fileName, 0);
		cv::imshow("fOrig", fOrig);

		cv::Mat fHist;
		cv::equalizeHist(fOrig, fHist);

		cv::imshow("fHist", fHist);
		cv::waitKey(10);

		// image processing
		cv::Mat filtered;
		cv::adaptiveThreshold(fHist, filtered, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 19, 19);
		cv::morphologyEx(filtered, filtered, cv::MORPH_OPEN, cv::Mat());

		

		fHist.convertTo(fHist, CV_64FC1);

		// Gabor Filtering
		cv::Mat fGabor = fHist.clone();

		int maxIterations = 36;
		double Sx = 0.5;
		double Sy = Sx;
		cv::Mat g;
		for (int m = 0; m < maxIterations; m++)
		{
			double theta = m * 2 * CV_PI / maxIterations;
			cv::Mat gaborKernel = cv::getGaborKernel(fHist.size(), Sx, theta, 1, 1);
			
			cv::filter2D(fHist, g, CV_64FC1, gaborKernel);
			fGabor = g + fGabor;

			//cv::imshow("fGabor", fGabor);
			//cv::waitKey();
		}

		cv::imshow("Gabor Filtered", fGabor);
		
		// adaptive binarization
		int windowSize = 32;
		int numRows = fGabor.rows;
		int numCols = fGabor.cols;
		int nC = std::ceil(numCols * 1.0 / windowSize);
		int nR = std::ceil(numRows * 1.0 / windowSize);

		cv::Mat fBin = cv::Mat::zeros(fGabor.size(), CV_8UC1);

		for (int m = 0; m < nC; m++)
		{
			for (int n = 0; n < nR; n++)
			{
				cv::Rect blockROI = cv::Rect(m * windowSize, n * windowSize,
					std::min(windowSize, numCols - m * windowSize),
					std::min(windowSize, numRows - n * windowSize));
				
				cv::Mat block;
				fGabor(blockROI).convertTo(block, CV_8UC1);
				cv::Scalar blockAvg = cv::mean(block);
				fBin(blockROI) = (block < blockAvg.val[0]);

				//if (windowSize > blockROI.height)
				//{
					//cv::Mat temp1,temp2;
					//block.convertTo(temp1, CV_8UC1);
					//fBin(blockROI).convertTo(temp2, CV_8UC1);
					//std::cout << blockROI << std::endl; 
					//std::cout << temp1 << std::endl;
					//std::cout << "\tAverage = " << blockAvg << std::endl;
					//std::cout << temp2 << std::endl;
					//std::cout << "**********" << std::endl;
				//}
				
			}
		}

		
		cv::imshow("fBin", ~fBin);

		// Image Thinning, ridges shown as black
		cv::Mat fThin;
		thinning(fBin, fThin);
		cv::imshow("fThin", ~fThin);

		////////////////// Find minutia, using a 3x3 window as described in class//////////////


		cv::waitKey();
	}
};