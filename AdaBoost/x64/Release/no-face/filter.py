import cv2
import numpy as np
import glob
import cv2
import os

files = glob.glob("*.jpg");

faceCascade = cv2.CascadeClassifier('E:\\MostafaIzz\\AdaBoost\\AdaBoost\\opencv\\etc\\haarcascades\\haarcascade_frontalface_alt2.xml')

for f in files:
	im = cv2.imread(f);
	if(im is not None and (im.shape[0] * im.shape[1]) > 0):
		gray = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
		faces = faceCascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(24, 24), flags = cv2.cv.CV_HAAR_SCALE_IMAGE)
		if len(faces) > 0:
			os.rename(f, './img/' + f);
			print './img/' + f
