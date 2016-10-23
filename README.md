# AdaBoost and Viola Jones

To open this project you will need Visual Studio 2015.

There are two projects in the solution:

1- The main code is C++ with OpenCV 3.1 compiled with Visual C++ 2015.

2- The other project "AdaBoostGUI" is GUI only, it is written in C# with WPF interface and linked to DLL from the firsrt project and linked to a precompiled OpenCV executable file for training cascade classifiers.


To run the project:

1- You can download the whole project or download the folder x64/Release.

2- Run the "AdaBoostGUI" tool. And you will find 3 tabs:

![Alt text](/imgs/0.jpg?raw=true "AdaBoostGUI")

Face databases:
http://www.face-rec.org/databases/

How to train OpenCV Cascade Classifier:
https://codeyarns.com/2014/09/01/how-to-train-opencv-cascade-classifier/
