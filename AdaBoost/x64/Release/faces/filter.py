import cv2
import numpy as np
import glob

files = glob.glob("*.jpg");

oup = open('ann.txt', 'w')

faceCascade = cv2.CascadeClassifier('C:/Users/mostafaizz/Source/Repos/AdaBoost/opencv/etc/haarcascades/haarcascade_frontalface_alt2.xml')
cnt = 0;
for f in files:
	im = cv2.imread(f);
	if(im is not None and (im.shape[0] * im.shape[1]) > 0):
		gray = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
		faces = faceCascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(30, 30), flags = cv2.cv.CV_HAAR_SCALE_IMAGE)
		if len(faces) > 0:
			cnt += len(faces)
			oup.write(f + ' ')
			oup.write(str(len(faces)) + ' ')
			for face in faces:
				oup.write(str(face[0]) + ' '+ str(face[1]) + ' '+ str(face[2]) +' '+ str(face[3]) + '\n')

oup.close()
print cnt