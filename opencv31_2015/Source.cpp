#include <opencv2\opencv.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\ml.hpp>
#include <opencv2\objdetect.hpp>
#include <vector>
#include <iostream>
#include <random>
#include <fstream>
#include <chrono>
#include <thread>
#include "AdaBoost.h"
#include "VectorData.h"
#include "MatData.h"
#include "WeakVectorClassifierFactory.h"
#include "WeakHaarClassifierFactory.h"
#include "AdaBoostEdgeDetector.h"

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
		std::vector<int> outLabels;
		double accuracy = adaboost.train(data, label, outLabels);
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
/*
void testMatData()
{
	cv::Mat m = cv::Mat::zeros(cv::Size(400, 400), CV_8UC1);
	m(cv::Rect(0, 0, 200, 200)) = 50;
	m(cv::Rect(0, 200, 200, 200)) = 200;
	m(cv::Rect(200, 0, 200, 200)) = 150;
	m(cv::Rect(200, 200, 200, 200)) = 100;
	cv::Mat Iimg, I2img;
	cv::integral(m, Iimg, I2img, CV_32FC1);
	// display
	//displayIntegralImage(Iimg);
	cv::Mat edges = AdaBoostEdgeDetector::cannyEdgeDetection(m);

	// prepare data vector
	std::vector<DataPoint*> trainData;
	std::vector<int> labels;
	cv::Mat show;
	cv::cvtColor(m, show, CV_GRAY2BGR);
	// make patches 10x10 with step 5
	for (int r = 0; r <= 390; r += 5)
	{
		for (int c = 0; c <= 390; c += 5)
		{
			cv::Rect win = cv::Rect(c, r, 10, 10);
			trainData.push_back(new MatData(Iimg, win));
			//MatData testData(m, win);
			//cv::imshow("window", testData.getMatData());
			//cv::waitKey();
			if (cv::sum(edges(win)).val[0] > 1)
			{
				// edge
				labels.push_back(1);
				cv::rectangle(show, win, cv::Scalar(255, 0, 0));
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
	std::vector<std::vector<int> > shape1(2,std::vector<int>(1,1));
	shape1[0][0] = -1;
	std::vector<std::vector<int> > shape2(2, std::vector<int>(2, 1));
	shape2[0][0] = shape2[1][1] = -1;


	shapes.push_back(std::vector<std::vector<int> >(1, std::vector<int>(arr, arr + 2)));
	shapes.push_back(shape1);
	//shapes.push_back(shape2);
	
	std::vector<cv::Point> locs(1, cv::Point(0, 0));
	std::vector<cv::Size> sizes;
	sizes.push_back(cv::Size(5, 10));
	sizes.push_back(cv::Size(10, 5));
	//sizes.push_back(cv::Size(5, 5));

	
	for (int i = 1; i < 30; i ++)
	{
		WeakClassifierFactory * factory = new WeakHaarClassifierFactory(shapes, sizes, locs);

		AdaBoost adaboost(i, factory);
		std::vector<int> y;
		double accuracy = adaboost.train(trainData, labels, y);
		std::cout << i << ", " << accuracy << std::endl;
		cv::Mat show1;
		show.copyTo(show1);
		for (int j = 0; j < y.size(); j++)
		{
			if (y[j] == 1)
			{
				if (labels[j] == 1)
				{
					cv::rectangle(show1, ((MatData*)trainData[j])->getROI(), cv::Scalar(0, 255, 0));
				}
				else
				{
					cv::rectangle(show1, ((MatData*)trainData[j])->getROI(), cv::Scalar(0, 0, 255));
				}
			}
		}
		cv::imshow("my edges", show1);
		cv::waitKey(0);
		//system("PAUSE");
	}
}
*/

unsigned char *getImageData(const cv::Mat img,int& size)
{
	std::vector<unsigned char> buf;
	cv::imencode(".png", img, buf);
	unsigned char *data = new unsigned char[buf.size()];
	memcpy(data, reinterpret_cast<unsigned char*>(buf.data()), buf.size());
	size = buf.size();
	return data;
}

extern "C"             //No name mangling
__declspec(dllexport)  //Tells the compiler to export the function
void 
__cdecl                //Specifies calling convention, cdelc is default, 
testAdaBoostEdgeDetection(int horizontal, int vertical, int t, char* trainImg, char* testImage,
	unsigned char* &trainImageData, int& trainImageSize, unsigned char* &testImageData, int& testImageSize)
{
	cv::Mat testImg = cv::imread(testImage, 0);

	cv::Mat m = cv::imread(trainImg, 0);
	std::vector<cv::Mat> images;
	images.push_back(m);

	std::vector<std::vector<std::vector<int> > > shapes;
	int arr[] = { -1,1 };
	std::vector<std::vector<int> > shape1(2, std::vector<int>(1, 1));
	shape1[0][0] = -1;
	std::vector<std::vector<int> > shape2(2, std::vector<int>(2, 1));
	shape2[0][0] = shape2[1][1] = -1;

	if (horizontal)
	{
		shapes.push_back(std::vector<std::vector<int> >(1, std::vector<int>(arr, arr + 2)));
	}
	if (vertical)
	{
		shapes.push_back(shape1);
	}
	//shapes.push_back(shape2);



	//for (int t = 1; t < 20; t++)
	//{
	AdaBoostEdgeDetector adaBoostEdgeDetector(t, shapes, cv::Size(4, 4), 2);

	std::vector<cv::Mat> tImgs = adaBoostEdgeDetector.train(images, false);
	cv::Mat testImg1 = adaBoostEdgeDetector.test(testImg, false);

	trainImageData = getImageData(tImgs[0], trainImageSize);
	testImageData = getImageData(testImg1, testImageSize);
	//}

}

extern "C"             //No name mangling
__declspec(dllexport)  //Tells the compiler to export the function
 void
__cdecl                //Specifies calling convention, cdelc is default, 
 test(int &size)
{
	for (size = 0; size < 10; size++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

extern "C"             //No name mangling
__declspec(dllexport)  //Tells the compiler to export the function
void
__cdecl
captureCamera(unsigned char* &image, int& size, int camID)
{
	size = 0;
	cv::VideoCapture cap;
	if (cap.open(camID))
	{
		cv::Mat img;
		if (cap.read(img))
		{
			image = getImageData(img, size);
		}
		cap.release();
	}
}

extern "C" __declspec(dllexport) unsigned char* __cdecl faceDetect(char* fileName, char *modelName, int &imgSize)
{
	imgSize = 0;
	cv::Mat img = cv::imread(fileName, 1);
	if (img.rows > 600)
	{
		cv::resize(img, img, cv::Size(img.cols * 600.0 / img.rows, 600));
	}
	

	cv::CascadeClassifier faceCascade;
	if (!faceCascade.load(modelName))
	{
		return 0;
	}
	cv::Mat gray;
	cv::cvtColor(img, gray, CV_BGR2GRAY);
	cv::equalizeHist(gray, gray);
	std::vector<cv::Rect> faces;
	faceCascade.detectMultiScale(gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
	for (int i = 0; i < faces.size(); i++)
	{
		cv::rectangle(img, faces[i], cv::Scalar(255, 0, 0),3);
	}
	//cv::imshow("faces", img);
	//cv::waitKey(50);
	return getImageData(img, imgSize);
}



int main(int argc, char*argv[])
{
	int horizontal = 1;
	int vertical = 1;
	int t = 6;
	char* trainImg = "orig.png";
	char* testImage = "test.png";
	for (int i = 0; i < argc; i++)
	{
		if (std::string(argv[i]) == "-h")
		{
			horizontal = std::stoi(argv[i + 1]);
		}
		else if (std::string(argv[i]) == "-v")
		{
			vertical = std::stoi(argv[i + 1]);
		}
		else if (std::string(argv[i]) == "-t")
		{
			t = std::stoi(argv[i + 1]);
		}
		else if (std::string(argv[i]) == "-i")
		{
			trainImg = argv[i + 1];
		}
		else if (std::string(argv[i]) == "-o")
		{
			testImage = argv[i + 1];
		}
		else
		{
			i--;
		}
		i++;
	}
	
	//int sz = 0;
	//unsigned char * data = test("orig.png",sz);
	//std::ofstream outfile("test.bmp", std::ofstream::binary);
	//outfile.write((char *)data, sz);
	//outfile.close();
	//test2DPoints();
	//testMatData();
	unsigned char * data1, *data2;
	int sz1, sz2;
	testAdaBoostEdgeDetection(horizontal,vertical,t,trainImg,testImage, data1, sz1, data2, sz2);
	//delete []data;
	return 0;
}
