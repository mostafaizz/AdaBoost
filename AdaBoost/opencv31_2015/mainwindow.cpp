#include "mainwindow.h"
#include "ui_mainwindow.h"
/*
#include <opencv2\opencv.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\ml.hpp>
#include <vector>
#include <iostream>
#include <random>
#include "AdaBoost.h"
#include "VectorData.h"
#include "MatData.h"
#include "WeakVectorClassifierFactory.h"
#include "WeakHaarClassifierFactory.h"
#include "AdaBoostEdgeDetector.h"

void testAdaBoostEdgeDetection(int horizontal, int vertical, int t, std::string trainImg,std::string testImage)
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
    {
        AdaBoostEdgeDetector adaBoostEdgeDetector(t, shapes, cv::Size(4,4), 2);
        adaBoostEdgeDetector.train(images, true);
        adaBoostEdgeDetector.test(testImg, true);
    }

}*/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
