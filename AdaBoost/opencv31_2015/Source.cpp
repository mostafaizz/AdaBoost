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
#include "AdaBoostCascadeClassifier.h"

const double PI = 3.14159265;

void test2DPoints()
{
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
		double accuracy = adaboost.train(data, label, outLabels, false);
		std::cout << i << ", " << accuracy << std::endl;
		//system("PAUSE");
	}
	cv::imshow("img", img);
	cv::waitKey();
}

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



void edgeDetection(int argc, char*argv[])
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
	
	unsigned char * data1, *data2;
	int sz1, sz2;
	testAdaBoostEdgeDetection(horizontal,vertical,t,trainImg,testImage, data1, sz1, data2, sz2);
}

extern "C" __declspec(dllexport)  AdaBoostCascadeClassifier* trainCascadeClassifier(
	char* posFile, char* negFile, 
	int numPositive, int numNegative,
	int patchWidth, int patchHeight, 
	double* sizeFactorsarr, int sizeFactorsLength,
	int* cascadeSizearr, int cascadeSizesLength, 
	int step)
{
	std::vector<int> cascadeSize(cascadeSizearr, cascadeSizearr + cascadeSizesLength);
	std::vector<double> sizeFactors(sizeFactorsarr, sizeFactorsarr + sizeFactorsLength);

	std::vector<std::vector<std::vector<int> > > shapes;
	
	int arr[] = { 1, -1,1 };
	std::vector<std::vector<int> > shape1;
	shape1.push_back(std::vector<int>(arr + 1, arr + 3));
	std::vector<std::vector<int> > shape2(2, std::vector<int>(1, 1));
	shape2[0][0] = -1;
	std::vector<std::vector<int> > shape3;
	shape3.push_back(std::vector<int>(arr, arr + 3));
	std::vector<std::vector<int> > shape4;
	shape4.push_back(std::vector<int>(arr, arr + 2));
	shape4.push_back(std::vector<int>(arr + 1, arr + 3));

	shapes.push_back(shape1);
	shapes.push_back(shape2);
	shapes.push_back(shape3);
	shapes.push_back(shape4);

	AdaBoostCascadeClassifier *classifier = new AdaBoostCascadeClassifier(cascadeSize, shapes, sizeFactors, cv::Size(patchWidth, patchHeight), step);

	std::vector<cv::Mat> trainImages;
	std::vector<int> labels;

	// first read the positive samples
	std::ifstream posReader;
	posReader.open(posFile);
	std::string posFolderName = posFile;
	posFolderName = posFolderName.substr(0,posFolderName.find_last_of("/") + 1);
	while (!posReader.eof())
	{
		std::string imgName;
		posReader >> imgName;
		cv::Mat tmpImg = cv::imread(posFolderName + imgName, 0);
		int n;
		posReader >> n;
		for (int i = 0; i < n; i++)
		{
			cv::Rect roi;
			posReader >> roi.x >> roi.y >> roi.width >> roi.height;
			cv::Mat trainImg;
			trainImg = tmpImg(roi).clone();
			trainImages.push_back(trainImg);
			labels.push_back(1);
			if (labels.size() == numPositive)
			{
				break;
			}
		}
		if (labels.size() == numPositive)
		{
			break;
		}
	}
	posReader.close();
	std::cout << "Positive Samples: " << labels.size() << std::endl;
	// read the negative images
	// first read the positive samples
	std::ifstream negReader;
	negReader.open(negFile);
	std::string negFolderName = negFile;
	negFolderName = negFolderName.substr(0, negFolderName.find_last_of("/") + 1);
	std::vector<cv::Mat> neg;
	while (!negReader.eof())
	{
		std::string line;
		std::getline(negReader, line);
		if (line.size() == 0)
		{
			break;
		}
		cv::Mat tmpImg = cv::imread(negFolderName + line, 0);
		neg.push_back(tmpImg);
	}
	negReader.close();

	for (int i = 0; i < numNegative; i++)
	{
		int imgID = rand() % neg.size();
		int x = rand() % (neg[imgID].cols - patchWidth);
		int y = rand() % (neg[imgID].rows - patchHeight);

		cv::Mat negImg;
		negImg = neg[imgID](cv::Rect(x, y, patchWidth, patchHeight)).clone();

		trainImages.push_back(negImg);
		labels.push_back(-1);
	}

	std::vector<int> y = classifier->train(trainImages, labels);
	
	return classifier;
}

extern "C" __declspec(dllexport) unsigned char* testCascadeClassifier(AdaBoostCascadeClassifier* classifier, char *imageName, int & retSize)
{
	cv::Mat testImg = cv::imread(imageName, 1);
	if (testImg.cols > 800)
	{
		cv::resize(testImg, testImg, cv::Size(800, testImg.rows * (800.0 / testImg.cols)));
	}
	cv::Mat gray;
	cv::cvtColor(testImg, gray, CV_BGR2GRAY);

	std::vector<cv::Rect> res = classifier->test(gray, { 2 }, false);
	std::cout << "Detected Objects: " << res.size() << std::endl;
	for (int i = 0; i < res.size(); i++)
	{
		cv::rectangle(testImg, res[i], cv::Scalar(255, 0, 0), 3);
	}
	//cv::imshow("image", testImg);
	//cv::waitKey();
	return getImageData(testImg, retSize);
}

extern "C" __declspec(dllexport) void deleteCascadeClassifier(AdaBoostCascadeClassifier* classifier)
{
	delete classifier;
}

int main(int argc, char **argv)
{
	//test2DPoints();
	//edgeDetection(argc, argv);
	std::vector<int> cascadeSizes = { 1, 2, 4, 8};
	std::vector<double> sizesFactors = {1, 0.6,  0.4 };
	AdaBoostCascadeClassifier* classifier =
		trainCascadeClassifier(
			"../x64/Release/faces/ann.txt",
			"../x64/Release/bg.txt",
			20, 100, 24, 24,
			&sizesFactors[0], sizesFactors.size(),
			&cascadeSizes[0], cascadeSizes.size(),
			3);
	int size = 0;
	unsigned char* retImg = testCascadeClassifier(classifier, "../x64/Release/faces/1.jpg", size);
	return 0;
}
