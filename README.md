# AdaBoost and Viola Jones

To open this project you will need Visual Studio 2015.

There are two projects in the solution:

1- The main code is C++ with OpenCV 3.1 compiled with Visual C++ 2015.

2- The other project "AdaBoostGUI" is GUI only, it is written in C# with WPF interface and linked to DLL from the firsrt project and linked to a precompiled OpenCV executable file for training cascade classifiers.


To run the project:

1- You can download the whole project or download the folder AdaBoost/x64/Release.

2- Run the "AdaBoostGUI" tool. And you will find 3 tabs:

a- The first tab is a demo for my edge detection usign AdaBoost and Haar Wavlets. You will need to select training and testing images (in .png format) and choose horizontal or vertical wavelets. Then hit run. (This code can be updated to detect faces, but I didn't have enough time to test this for now).

![Alt text](/imgs/0.jpg?raw=true "AdaBoost Edge Detection")

b- The second tab is interface for the opencv face detection algorithm. You need to select the image (jpg or png). And select the model, either an OpenCV pretrained model from opencv "etc" directory. OR you can use your trained model. Then hit "Detect Face".

![Alt text](/imgs/1.jpg?raw=true "Detect Faces")

c- Third tab is used for training the Cascade Classifier (our focus on faces now). This tab has different inputs:
* Positive Samples file (*.vec): this is a binary file containing the positive samples we need for training. This file needs preparation before training and I will mention this later. For now, I have prepared one file called "pos-samples.vec" it has more than 2000 face images, each face is 24x24 pixels (which are the patch width and height)

* negative samples file: This is a text file containing paths for images that does not contain faces. Each line is path for one image. You can use any images, but I have prepared a folder "no-face" and a file "bg.txt" that can be used directly.

* output folder: You need to select the ouptut folder for your classifer before starting. Any empty folder should be fine. This folder will be used to save the classifier with the name "Cascade.xml" at the end + different files describing the stages for the cascaded training.

* Patch Width and Patch Height: must be the same as the sizes in the positive samples file ".vec" (here it was prepared to be 24x24 but you can use any size if you prepared different data).

* Number of Positive Samples: This must be less than or equal to the images in the positive samples file ".vec" (In the file I have prepared there are at least 2800 faces)

* Number of negative samples: Can be any number, because the application takes random patches from the given negative images files. (But each image must be at least the size of the positive patch which is 24x24 in the given data)

* the other parameters you can check them in the OpenCV page: http://docs.opencv.org/trunk/dc/d88/tutorial_traincascade.html

After that you need to start trainging and be patient, because it takes long time unless you are using small number of samples.

![Alt text](/imgs/2.jpg?raw=true "Train Viola Jones")

And you will find the classifier output in the folder specified at the end.

![Alt text](/imgs/3.jpg?raw=true "Viola Jones training output")


To compile the Project you will need to make sure you downloaded the whole directory. And open the solution file "opencv31_2015.sln" with visual studio 2015, then change the configuration to x64.

![Alt text](/imgs/4.jpg?raw=true "Visual Studio")

Also, make sure to make the AdaBoostGUI project as the startup project

![Alt text](/imgs/5.jpg?raw=true "Visual Studio")

To prepare positive samples, you will need face data base,
either from this link: http://www.face-rec.org/databases/

Or you can use your own images.

Then, you need to annotate the images in a text file, each line has path of an image followed by a space, then the number of faces, then space separated recatngles. 

Example:

3.jpg 5 134 296 44 47 308 180 20 18 19 169 28 25 524 289 32 32 197 217 20 25

The image "3.jpg" has 5 faces, the first face starts at location (x,y) = (134,269) and  width = 44, height = 47.

The faces can be of any size because the next step will normalize the faces to the needed size.

The other option to do annotation is to use opencv tool "opencv_annotation" (in the same Release directory we are working on). The inputs to this tool are images directory and output file. It displays the image and you can create rectangle and press "c" to confirm your selection. After you are done with one image you can press "n" to go to the next image.

For the last step, to prepare the binary ".vec" file that contains the samples, you can follow the tutorial at:
https://codeyarns.com/2014/09/01/how-to-train-opencv-cascade-classifier/


For Question and Comments please contact Mostafa Izz:
mostafaizz1408@gmail.com
