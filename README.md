# AdaBoost and Viola Jones

To open this project you will need Visual Studio 2015.

There are two projects in the solution:

1- The main code is C++ with OpenCV 3.1 compiled with Visual C++ 2015.

2- The other project "AdaBoostGUI" is GUI only, it is written in C# with WPF interface and linked to DLL from the firsrt project and linked to a precompiled OpenCV executable file for training cascade classifiers.


To run the project:

1- You can download the whole project or download the folder x64/Release.

2- Run the "AdaBoostGUI" tool. And you will find 3 tabs:

a- The first tab is a demo for my edge detection usign AdaBoost and Haar Wavlets. You will need to select training and testing images (in .png format) and choose horizontal or vertical wavelets. Then hit run. (This code can be updated to detect faces, but I didn't have enough time to test this for now).

![Alt text](/imgs/0.jpg?raw=true "AdaBoost Edge Detection")

b- The second tab is interface for the opencv face detection algorithm. You need to select the image (jpg or png). And select the model, either an OpenCV pretrained model from opencv "etc" directory. OR you can use your trained model. Then hit "Detect Face".

![Alt text](/imgs/1.jpg?raw=true "Detect Fcaes")

Face databases:
http://www.face-rec.org/databases/

How to train OpenCV Cascade Classifier:
https://codeyarns.com/2014/09/01/how-to-train-opencv-cascade-classifier/
