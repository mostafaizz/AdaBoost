#include <opencv2\opencv.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\ml.hpp>
#include <opencv2\objdetect.hpp>
#include <vector>
#include <iostream>
#include <random>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include "AdaBoost.h"
#include "VectorData.h"
#include "MatData.h"
#include "WeakVectorClassifierFactory.h"
#include "WeakHaarClassifierFactory.h"
#include "AdaBoostEdgeDetector.h"
#include "AdaBoostCascadeClassifier.h"
#include "IrisRecognizer.h"
#include "FingerPrint.h"

const double PI = 3.14159265;


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
unsigned char* test2DPoints(int &retSize, int& numClassifiers, double& accuracy,int maximum = 30, double percenatge = 0.95)
{
	// generate the data
	std::vector<DataPoint*> data;
	int i = 0;
	cv::Mat img = cv::Mat::zeros(1100, 1100, CV_32FC3);
	for (double r = 0.1 * 1000; r <= 0.51 * 1000; r += 0.05 * 1000)
	{
		for (double theta = 0; theta <= 1.81 * PI; theta += (PI / 5), i++)
		{
			std::vector<double> d = std::vector<double>(2, 0);
			d[0] = 0.5f + r * std::cos(theta);
			d[1] = 0.5f + (r - 0.05) * std::sin(theta);

			data.push_back(new VectorData(d));
		}
	}

	std::cout << data.size() << std::endl;
	int shift = 510;
	std::vector<int> label(30, 1);
	label.resize(data.size(), -1);
	for (int i = 0; i < data.size(); i++)
	{
		cv::circle(img, cv::Point(data[i]->getVectorData()[0] + shift, data[i]->getVectorData()[1] + shift),
			3, cv::Scalar(((label[i] - 1) / -2) * 255, 0, ((label[i] + 1) / 2) * 255), -1);
	}
	//freopen("result.csv", "w", stdout);
	WeakClassifierFactory * factory = new WeakVectorClassifierFactory();
	std::vector<int> outLabels;
	for (numClassifiers = 1; numClassifiers < maximum; numClassifiers += 2)
	{
		AdaBoost adaboost(numClassifiers, factory);
		outLabels.clear();
		accuracy = adaboost.train(data, label, outLabels, false);
		
		if (accuracy >= percenatge)
		{
			break;
		}
		std::cout << numClassifiers << ", " << accuracy << std::endl;
		//system("PAUSE");
	}
	//cv::imshow("img", img);
	//cv::waitKey();
	for (int i = 0; i < data.size(); i++)
	{
		if (outLabels[i] * label[i] < 0)
		{
			cv::circle(img,
				cv::Point(data[i]->getVectorData()[0] + shift, data[i]->getVectorData()[1] + shift),
				8, cv::Scalar(0, 0, 255), 2);
		}
	}
	return getImageData(img, retSize);
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

enum
{
	SOBEL3,
	SOBEL5,
	MEDIAN5,
	MEDIAN9,
	GAUSS5,
	GAUSS9
};

cv::Mat applySobel(cv::Mat &img, int size)
{
	/// Generate grad_x and grad_y
	cv::Mat grad_x, grad_y, grad;
	cv::Mat abs_grad_x, abs_grad_y;

	/// Gradient X
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	Sobel(img, grad_x, CV_16S, 1, 0, size);
	convertScaleAbs(grad_x, abs_grad_x);
	
	/// Gradient Y
	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	Sobel(img, grad_y, CV_16S, 0, 1, size);
	convertScaleAbs(grad_y, abs_grad_y);

	/// Total Gradient (approximate)
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

	return grad;
}

extern "C" __declspec(dllexport) unsigned char* applyOperator(char*imageName, int* ops, int opsSize, int &retSize)
{
	cv::Mat img = cv::imread(imageName,0);
	cv::Mat test = img.clone();
	for (int i = 0; i < opsSize; i++)
	{
		switch (ops[i])
		{
		case SOBEL3:
			test = applySobel(test, 3);
			break;
		case SOBEL5:
			test = applySobel(test, 5);
			break;
		case MEDIAN5:
			cv::medianBlur(test, test, 5);
			break;
		case MEDIAN9:
			cv::medianBlur(test, test, 9);
			break;
		case GAUSS5:
			cv::GaussianBlur(test, test, cv::Size(5, 5), 0);
			break;
		case GAUSS9:
			cv::GaussianBlur(test, test, cv::Size(9, 9), 0);
			break;
		default:
			break;
		}
	}

	return getImageData(test, retSize);
}

extern "C" __declspec(dllexport) unsigned char * getColorHist(char* imageName, int& retSize)
{
	cv::Mat src, dst;

	/// Load image
	src = cv::imread(imageName, 1);

	if (!src.data)
	{
		retSize = 0;
		return 0;
	}

	/// Separate the image in 3 places ( B, G and R )
	std::vector<cv::Mat> bgr_planes;
	split(src, bgr_planes);

	/// Establish the number of bins
	int histSize = 256;

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 };
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;

	cv::Mat b_hist, g_hist, r_hist;

	/// Compute the histograms:
	calcHist(&bgr_planes[0], 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[1], 1, 0, cv::Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[2], 1, 0, cv::Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);

	// Draw the histograms for B, G and R
	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	cv::normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
	cv::normalize(g_hist, g_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
	cv::normalize(r_hist, r_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

	/// Draw for each channel
	for (int i = 1; i < histSize; i++)
	{
		cv::line(histImage, cv::Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
			cv::Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
			cv::Scalar(255, 0, 0), 2, 8, 0);
		cv::line(histImage, cv::Point(bin_w*(i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
			cv::Point(bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))),
			cv::Scalar(0, 255, 0), 2, 8, 0);
		cv::line(histImage, cv::Point(bin_w*(i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
			cv::Point(bin_w*(i), hist_h - cvRound(r_hist.at<float>(i))),
			cv::Scalar(0, 0, 255), 2, 8, 0);
	}

	return getImageData(histImage, retSize);
}

extern "C" __declspec(dllexport) unsigned char * getGrayHist(char* imageName, int& retSize)
{
	cv::Mat src, dst;

	/// Load image
	src = cv::imread(imageName, 0);

	if (!src.data)
	{
		retSize = 0;
		return 0;
	}

	/// Establish the number of bins
	int histSize = 256;

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 };
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;

	cv::Mat gray_hist;

	/// Compute the histograms:
	calcHist(&src, 1, 0, cv::Mat(), gray_hist, 1, &histSize, &histRange, uniform, accumulate);
	
	// Draw the histograms for B, G and R
	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	cv::normalize(gray_hist, gray_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
	
	/// Draw for each channel
	for (int i = 1; i < histSize; i++)
	{
		cv::line(histImage, cv::Point(bin_w*(i - 1), hist_h - cvRound(gray_hist.at<float>(i - 1))),
			cv::Point(bin_w*(i), hist_h - cvRound(gray_hist.at<float>(i))),
			cv::Scalar(255, 255, 255), 2, 8, 0);
		
	}

	return getImageData(histImage, retSize);
}


extern "C" __declspec(dllexport) void deleteCascadeClassifier(AdaBoostCascadeClassifier* classifier)
{
	delete classifier;
}

int testCascadeClassifier(int argc, char **argv)
{
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

void testIrisRecognizer()
{
	IrisRecognizer iRec(cv::Size(512, 64));
	for (int i = 1; i <= 50; i++)
	{
		char buf[100];
		std::sprintf(buf, "../IrisImages/%d.jpg", i);
		iRec.IrisRecognizerRead(buf);
		//iRec.showCurrentEyeImage();
		iRec.extractPupil();
		cv::Mat strip = iRec.extractIris();
		cv::Mat code =  iRec.calcFeatures();
		cv::Mat orig = iRec.getOrigImage();
		cv::imshow("Original Image", orig);
		cv::imshow("Normalized IRIS", strip);
		cv::imshow("Code", code);
		cv::waitKey();
	}
	//cv::imshow("pupilMask", pupilMask);
	//cv::waitKey();

}

extern "C" __declspec(dllexport)  IrisRecognizer* createIrisRecognizer(int w, int h)
{
	IrisRecognizer* iRec = new IrisRecognizer(cv::Size(w, h));

	return iRec;
}

extern "C" __declspec(dllexport) void extractIris(char * imgFileName, IrisRecognizer* iRec,
	unsigned char* &stripData, int& stripSize,
	unsigned char* &origData, int& origSize)
{
	iRec->IrisRecognizerRead(imgFileName);
	iRec->extractPupil();
	cv::Mat strip = iRec->extractIris();
	stripData = getImageData(strip, stripSize);
	cv::Mat orig = iRec->getOrigImage();
	origData = getImageData(orig, origSize);
}

extern "C" __declspec(dllexport) void extractIrisFeatures(IrisRecognizer* iRec,
	unsigned char* &codeData, int& codeSize)
{
	cv::Mat code = iRec->calcFeatures();
	codeData = getImageData(code, codeSize);
}

extern "C" __declspec(dllexport) void deleteIrisRecognizer(IrisRecognizer* iRec)
{
	delete iRec;
}


extern "C" __declspec(dllexport) FingerPrintRecognizer* createFingerPrintMatcher()
{
	return new FingerPrintRecognizer();
}

extern "C" __declspec(dllexport) void deleteFingerPrintMatcher(FingerPrintRecognizer* ptr)
{
	delete ptr;
}

extern "C" __declspec(dllexport) void trainFingerPrintOneImage(FingerPrintRecognizer* ptr, char * name, char* fileName)
{
	ptr->trainOneFinger(name, fileName);
}

extern "C" __declspec(dllexport) unsigned char* computeFingerFeatures(FingerPrintRecognizer* ptr, char* fileName, int& retSize)
{
	cv::Mat img = ptr->computeFeatForTesting(fileName);
	return getImageData(img, retSize);
}

extern "C" __declspec(dllexport) double testFingerPrintOneImage(FingerPrintRecognizer* ptr, char * name)
{
	return ptr->testImage(name);
}


int main(int argc, char **argv)
{
	//test2DPoints();
	//edgeDetection(argc, argv);
	//testCascadeClassifier(argc, argv);
	//testIrisRecognizer();
	//IrisRecognizer* iRec = createIrisRecognizer(512, 64);

	FingerPrintRecognizer fpR;
	for (int i = 1; i < 10; i++)
	{
		std::ostringstream osr;
		osr << "C:/Users/mostafaizz/Source/Repos/AdaBoost/AdaBoost/x64/Release/fp_Images/training/gallery" << i << ".bmp";
		std::cout << osr.str() << std::endl;
		fpR.trainOneFinger(std::to_string(i), osr.str());
	}

	std::ostringstream osr1;
	osr1 << "C:/Users/mostafaizz/Source/Repos/AdaBoost/AdaBoost/x64/Release/fp_Images/testing/test13.bmp";
	int retSize = 0;
	computeFingerFeatures(&fpR, (char*) (osr1.str().c_str()), retSize);
	cv::Mat display = fpR.computeFeatForTesting(osr1.str());

	cv::imshow("display", display); 
	cv::waitKey();
	for (int i = 1; i < 10; i++)
	{
		
		double score = fpR.testImage(std::to_string(i));

		std::cout << i << "\t" << score << std::endl;
	}
	return 0;
}