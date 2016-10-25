#include "Util.h"



Util::Util()
{
}

double Util::getRectangleSum(cv::Mat & iImg, cv::Rect win)
{
	double A = iImg.at<float>(cv::Point(win.x + win.width - 1, win.y + win.height - 1));
	double B = iImg.at<float>(cv::Point(win.x, win.y + win.height - 1));
	double C = iImg.at<float>(cv::Point(win.x + win.width - 1, win.y));
	double D = iImg.at<float>(cv::Point(win.x, win.y));

	return (A - B - C + D) / (win.width * win.height);
}


void Util::displayIntegralImage(cv::Mat img)
{
	static int count = 0;
	cv::Mat Iimg = img.clone();
	double min, max;
	cv::minMaxLoc(Iimg, &min, &max);
	Iimg -= min;
	Iimg /= (max - min);
	cv::imshow("image" + std::to_string(count++), Iimg);
	cv::waitKey();
}

Util::~Util()
{
}
