#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <map>

#define DISPLAY_ 0
class FingerPrintRecognizer
{
	std::map<std::string, cv::Mat> dictionary;
	cv::Mat testSubjectDescriptor;
public:

	// Perform a single thinning iteration, which is repeated until the skeletization is finalized
	void thinningIteration(cv::Mat& im, int iter)
	{
		cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);
		for (int i = 1; i < im.rows - 1; i++)
		{
			for (int j = 1; j < im.cols - 1; j++)
			{
				uchar p2 = im.at<uchar>(i - 1, j);
				uchar p3 = im.at<uchar>(i - 1, j + 1);
				uchar p4 = im.at<uchar>(i, j + 1);
				uchar p5 = im.at<uchar>(i + 1, j + 1);
				uchar p6 = im.at<uchar>(i + 1, j);
				uchar p7 = im.at<uchar>(i + 1, j - 1);
				uchar p8 = im.at<uchar>(i, j - 1);
				uchar p9 = im.at<uchar>(i - 1, j - 1);

				int A = (p2 == 0 && p3 == 1) + (p3 == 0 && p4 == 1) +
					(p4 == 0 && p5 == 1) + (p5 == 0 && p6 == 1) +
					(p6 == 0 && p7 == 1) + (p7 == 0 && p8 == 1) +
					(p8 == 0 && p9 == 1) + (p9 == 0 && p2 == 1);
				int B = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
				int m1 = iter == 0 ? (p2 * p4 * p6) : (p2 * p4 * p8);
				int m2 = iter == 0 ? (p4 * p6 * p8) : (p2 * p6 * p8);

				if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
					marker.at<uchar>(i, j) = 1;
			}
		}

		im &= ~marker;
	}

	// Function for thinning any given binary image within the range of 0-255. If not you should first make sure that your image has this range preset and configured!
	void thinning(cv::Mat& im)
	{
		// Enforce the range tob e in between 0 - 255
		im /= 255;

		cv::Mat prev = cv::Mat::zeros(im.size(), CV_8UC1);
		cv::Mat diff;

		do {
			thinningIteration(im, 0);
			thinningIteration(im, 1);
			absdiff(im, prev, diff);
			im.copyTo(prev);
		} while (countNonZero(diff) > 0);

		im *= 255;
	}

	void run1(const char * fileName)
	{
		{
			// Read in an input image - directly in grayscale CV_8UC1
			// This will be our test fingerprint
			cv::Mat input = cv::imread(fileName, cv::IMREAD_GRAYSCALE);

			// Binarize the image, through local thresholding
			cv::Mat input_binary;
			threshold(input, input_binary, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);

			// Compare both
			cv::Mat container(input.rows, input.cols * 2, CV_8UC1);
			input.copyTo(container(cv::Rect(0, 0, input.cols, input.rows)));
			input_binary.copyTo(container(cv::Rect(input.cols, 0, input.cols, input.rows)));
			
			cv::imshow("input versus binary", container);
			cv::waitKey(0);

			// Now apply the thinning algorithm
			cv::Mat input_thinned = input_binary.clone();
			thinning(input_thinned);

			// Compare both
			input_binary.copyTo(container(cv::Rect(0, 0, input.cols, input.rows)));
			input_thinned.copyTo(container(cv::Rect(input.cols, 0, input.cols, input.rows)));
			imshow("binary versus thinned", container); 
			cv::waitKey(0);

			// Now lets detect the strong minutiae using Haris corner detection
			cv::Mat harris_corners, harris_normalised;
			harris_corners = cv::Mat::zeros(input_thinned.size(), CV_32FC1);
			cv::cornerHarris(input_thinned, harris_corners, 2, 3, 0.04, cv::BORDER_DEFAULT);
			cv::normalize(harris_corners, harris_normalised, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());

			// Select the strongest corners that you want
			int threshold_harris = 125;
			std::vector<cv::KeyPoint> keypoints;

			// Make a color clone for visualisation purposes
			cv::Mat rescaled;
			cv::convertScaleAbs(harris_normalised, rescaled);
			cv::Mat harris_c(rescaled.rows, rescaled.cols, CV_8UC3);
			cv::Mat in[] = { rescaled, rescaled, rescaled };
			int from_to[] = { 0,0, 1,1, 2,2 };
			cv::mixChannels(in, 3, &harris_c, 1, from_to, 3);

			for (int x = 0; x<harris_normalised.cols; x++) {
				for (int y = 0; y<harris_normalised.rows; y++) {
					if ((int)harris_normalised.at<float>(y, x) > threshold_harris) {
						// Draw or store the keypoint location here, just like you decide. In our case we will store the location of the keypoint
						circle(harris_c, cv::Point(x, y), 5, cv::Scalar(0, 255, 0), 1);
						circle(harris_c, cv::Point(x, y), 1, cv::Scalar(0, 0, 255), 1);
						keypoints.push_back(cv::KeyPoint(x, y, 1));
					}
				}
			}

			cv::imshow("temp", harris_c); 
			cv::waitKey(0);

			// Compare both
			cv::Mat ccontainer(input.rows, input.cols * 2, CV_8UC3);
			cv::Mat input_thinned_c = input_thinned.clone();
			cv::cvtColor(input_thinned_c, input_thinned_c, CV_GRAY2BGR);
			input_thinned_c.copyTo(ccontainer(cv::Rect(0, 0, input.cols, input.rows)));
			harris_c.copyTo(ccontainer(cv::Rect(input.cols, 0, input.cols, input.rows)));
			
			cv::imshow("thinned versus selected corners", ccontainer); 
			cv::waitKey(0);

			// Calculate the ORB descriptor based on the keypoint
			cv::Ptr<cv::Feature2D> orb_descriptor = cv::ORB::create();
			cv::Mat descriptors;
			orb_descriptor->compute(input_thinned, keypoints, descriptors);

			// You can now store the descriptor in a matrix and calculate all for each image.
			// Since we just got the hamming distance brute force matching left, we will take another image and calculate the descriptors also.
			// Removed as much overburden comments as you can find them above
			cv::Mat input2 = cv::imread(fileName, cv::IMREAD_GRAYSCALE);
			cv::Mat input_binary2;
			cv::threshold(input2, input_binary2, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
			cv::Mat input_thinned2 = input_binary2.clone();
			thinning(input_thinned2);
			cv::Mat harris_corners2, harris_normalised2;
			harris_corners2 = cv::Mat::zeros(input_thinned2.size(), CV_32FC1);
			cv::cornerHarris(input_thinned2, harris_corners2, 2, 3, 0.04, cv::BORDER_DEFAULT);
			cv::normalize(harris_corners2, harris_normalised2, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());
			std::vector<cv::KeyPoint> keypoints2;
			cv::Mat rescaled2;
			cv::convertScaleAbs(harris_normalised2, rescaled2);
			cv::Mat harris_c2(rescaled2.rows, rescaled2.cols, CV_8UC3);
			cv::Mat in2[] = { rescaled2, rescaled2, rescaled2 };
			int from_to2[] = { 0,0, 1,1, 2,2 };
			cv::mixChannels(in2, 3, &harris_c2, 1, from_to2, 3);

			for (int x = 0; x<harris_normalised2.cols; x++) {
				for (int y = 0; y<harris_normalised2.rows; y++) {
					if ((int)harris_normalised2.at<float>(y, x) > threshold_harris) {
						// Draw or store the keypoint location here, just like you decide. In our case we will store the location of the keypoint
						circle(harris_c2, cv::Point(x, y), 5, cv::Scalar(0, 255, 0), 1);
						circle(harris_c2, cv::Point(x, y), 1, cv::Scalar(0, 0, 255), 1);
						keypoints2.push_back(cv::KeyPoint(x, y, 1));
					}
				}
			}
			cv::imshow("temp2", harris_c2); 
			cv::waitKey(0);

			cv::Mat descriptors2;
			orb_descriptor->compute(input_thinned2, keypoints2, descriptors2);

			// Now lets match both descriptors
			// Create the matcher interface
			cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("BruteForce-Hamming");
			std::vector< cv::DMatch > matches;
			matcher->match(descriptors, descriptors2, matches);

			// Loop over matches and multiply
			// Return the matching certainty score
			float score = 0.0;
			for (int i = 0; i < matches.size(); i++) {
				cv::DMatch current_match = matches[i];
				score = score + current_match.distance;
			}
			//std::cerr << std::endl << "Current matching score = " << score << std::endl;
		}
	}

	cv::Mat computeFeatures(const char* fileName)
	{
		// read gray image
		cv::Mat fOrig = cv::imread(fileName, 0);
		

		cv::Mat fHist;
		cv::equalizeHist(fOrig, fHist);

#if DISPLAY_
		cv::imshow("fOrig", fOrig);
		cv::imshow("fHist", fHist);
		cv::waitKey(10);
#endif
		// image processing
		cv::Mat filtered;
		cv::adaptiveThreshold(fHist, filtered, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 19, 19);
		cv::morphologyEx(filtered, filtered, cv::MORPH_OPEN, cv::Mat());

		

		filtered.convertTo(filtered, CV_64FC1);

		// Gabor Filtering
		cv::Mat fGabor = filtered.clone();

		int maxIterations = 72;
		double Sx = 0.5;
		double Sy = Sx;
		cv::Mat g;
		for (int m = 0; m < maxIterations; m++)
		{
			double theta = m * 2 * CV_PI / maxIterations;
			cv::Mat gaborKernel = cv::getGaborKernel(filtered.size(), Sx, theta, 1, 1);
			
			cv::filter2D(filtered, g, CV_64FC1, gaborKernel);
			fGabor = g + fGabor;

			//cv::imshow("fGabor", fGabor);
			//cv::waitKey();
		}
#if DISPLAY_
		cv::imshow("Gabor Filtered", fGabor);
#endif
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
			}
		}

		
		

		// Image Thinning, ridges shown as black
		cv::Mat fThin = fBin.clone();
		thinning(fThin);
#if DISPLAY_
		cv::imshow("fBin", ~fBin);
		cv::imshow("fThin", ~fThin);
#endif
		////////////////// Find minutia, using a 3x3 window as described in class//////////////
		// Now lets detect the strong minutiae using Haris corner detection
		cv::Mat harris_corners, harris_normalised;
		harris_corners = cv::Mat::zeros(fThin.size(), CV_32FC1);
		cv::cornerHarris(fThin, harris_corners, 2, 3, 0.04, cv::BORDER_DEFAULT);
		cv::normalize(harris_corners, harris_normalised, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());

		// Select the strongest corners that you want
		int threshold_harris = 125;
		std::vector<cv::KeyPoint> keypoints;

		// Make a color clone for visualisation purposes
		cv::Mat rescaled;
		cv::convertScaleAbs(harris_normalised, rescaled);
#if DISPLAY_
		cv::Mat harris_c(rescaled.rows, rescaled.cols, CV_8UC3);
		cv::Mat in[] = { rescaled, rescaled, rescaled };
		int from_to[] = { 0,0, 1,1, 2,2 };
		cv::mixChannels(in, 3, &harris_c, 1, from_to, 3);
#endif

		int xborder = 70;
		int yborder = 40;

		for (int x = xborder; x<harris_normalised.cols - xborder; x++) {
			for (int y = yborder; y<harris_normalised.rows - yborder; y++) {
				if ((int)harris_normalised.at<float>(y, x) > threshold_harris) {
					// Draw or store the keypoint location here, just like you decide. 
					// In our case we will store the location of the keypoint
#if DISPLAY_
					circle(harris_c, cv::Point(x, y), 5, cv::Scalar(0, 255, 0), 1);
					circle(harris_c, cv::Point(x, y), 1, cv::Scalar(0, 0, 255), 1);
#endif
					keypoints.push_back(cv::KeyPoint(x, y, 1));
				}
			}
		}

		//cv::imshow("temp", harris_c);
		//cv::waitKey(0);

		

		
		// Calculate the ORB descriptor based on the keypoint
		cv::Ptr<cv::Feature2D> orb_descriptor = cv::ORB::create();
		cv::Mat descriptors;
		orb_descriptor->compute(fThin, keypoints, descriptors);

#if DISPLAY_
		// Compare both
		cv::Mat ccontainer(fThin.rows, fThin.cols * 2, CV_8UC3);
		cv::Mat input_thinned_c = fThin.clone();
		cv::cvtColor(input_thinned_c, input_thinned_c, CV_GRAY2BGR);
		input_thinned_c.copyTo(ccontainer(cv::Rect(0, 0, fThin.cols, fThin.rows)));
		harris_c.copyTo(ccontainer(cv::Rect(fThin.cols, 0, fThin.cols, fThin.rows)));
		cv::imshow("thinned versus selected corners", ccontainer);
		cv::waitKey();
#endif
		std::cout << "Done !" << std::endl;
		return descriptors;
	}

	cv::Mat computeFeaturesORB(const char* fileName, cv::Mat& display, int displayFlag = false)
	{
		// read gray image
		cv::Mat fOrig = cv::imread(fileName, 0);

		cv::Mat fHist;
		cv::equalizeHist(fOrig, fHist);

#if DISPLAY_
		cv::imshow("fOrig", fOrig);
		cv::imshow("fHist", fHist);
		cv::waitKey(10);
#endif
		// image processing
		cv::Mat filtered;
		cv::adaptiveThreshold(fHist, filtered, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 19, 19);
		cv::morphologyEx(filtered, filtered, cv::MORPH_OPEN, cv::Mat());



		filtered.convertTo(filtered, CV_64FC1);

		// Gabor Filtering
		cv::Mat fGabor = filtered.clone();

		int maxIterations = 72;
		double Sx = 0.5;
		double Sy = Sx;
		cv::Mat g;
		for (int m = 0; m < maxIterations; m++)
		{
			double theta = m * 2 * CV_PI / maxIterations;
			cv::Mat gaborKernel = cv::getGaborKernel(filtered.size(), Sx, theta, 1, 1);

			cv::filter2D(filtered, g, CV_64FC1, gaborKernel);
			fGabor = g + fGabor;

			//cv::imshow("fGabor", fGabor);
			//cv::waitKey();
		}
#if DISPLAY_
		cv::imshow("Gabor Filtered", fGabor);
#endif
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
			}
		}




		// Image Thinning, ridges shown as black
		cv::Mat fThin = fBin.clone();
		thinning(fThin);
#if DISPLAY_
		cv::imshow("fBin", ~fBin);
		cv::imshow("fThin", ~fThin);
#endif
		////////////////// Find minutia, using a 3x3 window as described in class//////////////
		cv::ORB::Feature2D detector;
		std::vector<cv::KeyPoint> keypoints;

		int xborder = 70;
		int yborder = 40;
		cv::Mat mask = cv::Mat::zeros(fThin.size(), CV_8UC1);
		mask(cv::Rect(xborder, yborder, fThin.cols - xborder - xborder, fThin.rows - yborder - yborder)) = 255;

		// Calculate the ORB descriptor based on the keypoint
		cv::Ptr<cv::Feature2D> orb_descriptor = cv::ORB::create();
		cv::Mat descriptors;
		orb_descriptor->detectAndCompute(fThin, mask, keypoints, descriptors);

		if (displayFlag)
		{
			display = cv::Mat::zeros(fThin.rows, fThin.cols, CV_8UC3);
			cv::Mat in[] = { fThin, fThin, fThin };
			int from_to[] = { 0,0, 1,1, 2,2 };
			cv::mixChannels(in, 3, &display, 1, from_to, 3);

			for (int i = 0; i < keypoints.size(); i++)
			{
				cv::circle(display, keypoints[i].pt, 5, cv::Scalar(0, 255, 0), 1);
				cv::circle(display, keypoints[i].pt, 1, cv::Scalar(0, 0, 255), 1);
				float x = 5 * std::cos(keypoints[i].angle);
				float y = 5 * std::sin(keypoints[i].angle);
				cv::line(display, keypoints[i].pt, cv::Point(x,y), cv::Scalar(0, 0, 255));
			}
			
		}
	

#if DISPLAY_
		// Compare both
		cv::Mat ccontainer(fThin.rows, fThin.cols * 2, CV_8UC3);
		cv::Mat input_thinned_c = fThin.clone();
		cv::cvtColor(input_thinned_c, input_thinned_c, CV_GRAY2BGR);
		input_thinned_c.copyTo(ccontainer(cv::Rect(0, 0, fThin.cols, fThin.rows)));
		harris_c.copyTo(ccontainer(cv::Rect(fThin.cols, 0, fThin.cols, fThin.rows)));
		cv::imshow("thinned versus selected corners", ccontainer);
		cv::waitKey();
#endif
		std::cout << "Done !" << std::endl;
		return descriptors;
	}

	cv::Mat trainOneFinger(std::string name,std::string fileName)
	{
		cv::Mat oup;
		dictionary[name] = computeFeaturesORB(fileName.c_str(), oup, 1);
		return dictionary[name];
	}

	// must be called before calling testImage
	cv::Mat computeFeatForTesting(std::string fileName)
	{
		cv::Mat oup;
		testSubjectDescriptor = computeFeaturesORB(fileName.c_str(), oup, 1);
		return oup;
	}

	// return the distance
	double testImage(std::string expectedSubjectName)
	{
		if (testSubjectDescriptor.empty() || dictionary.find(expectedSubjectName) == dictionary.end())
		{
			return 100000000;
		}

		cv::Mat orig = dictionary[expectedSubjectName];
		// Now lets match both descriptors
		// Create the matcher interface
		cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("BruteForce-Hamming");
		std::vector< cv::DMatch > matches;
		matcher->match(orig, testSubjectDescriptor, matches);

		// Loop over matches and multiply
		// Return the matching certainty score
		double score = 0.0;
		for (int i = 0; i < matches.size(); i++) {
			cv::DMatch current_match = matches[i];
			score = score + current_match.distance;
		}
		std::cerr << std::endl << "Current matching score = " << score << std::endl;

		return score;
	}
};

