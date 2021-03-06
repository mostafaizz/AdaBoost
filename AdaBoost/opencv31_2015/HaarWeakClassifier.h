#pragma once
#include "WeakClassifier.h"
class HaarWeakClassifier :
	public WeakClassifier
{
protected:
	// width and height of a single part
	cv::Size origSize; 
	// x,y
	cv::Point origLocation; 
	// 1, -1, first diemnsion is the rows and second dimension is the columns
	std::vector<std::vector<int> > shape; 
public:
	HaarWeakClassifier(cv::Size,cv::Point,std::vector<std::vector<int> >,int direction);
	virtual int classify(DataPoint* data, double sizeFactor);
	virtual void operator=(const WeakClassifier& obj);
	virtual bool operator==(WeakClassifier* obj);
	virtual void print();
	virtual std::string getName();
	~HaarWeakClassifier();
};

