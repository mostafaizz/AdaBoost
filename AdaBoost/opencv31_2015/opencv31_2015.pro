#-------------------------------------------------
#
# Project created by QtCreator 2016-10-21T11:10:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = opencv31_2015
CONFIG += c++11
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    AdaBoost.h \
    AdaBoostEdgeDetector.h \
    DataPoint.h \
    HaarWeakClassifier.h \
    HaarWeakClassifierAbs.h \
    MatData.h \
    VectorData.h \
    WeakClassifier.h \
    WeakClassifierFactory.h \
    WeakHaarClassifierFactory.h \
    WeakVectorClassifierFactory.h

FORMS    += mainwindow.ui

INCLUDEPATH += C:/Users/mostafaizz/Downloads/opencv_test/opencv/include
CONFIG( debug, debug|release ) {
LIBS += "C:/Users/mostafaizz/Downloads/opencv_test/opencv31_2015/x64/Debug/opencv31_2015.lib"
LIBS += "C:/Users/mostafaizz/Downloads/opencv_test/opencv/x64/vc14/lib/opencv_world310d.lib"
}
else{
LIBS += "C:/Users/mostafaizz/Downloads/opencv_test/opencv/x64/vc14/lib/opencv_world310.lib"
}
