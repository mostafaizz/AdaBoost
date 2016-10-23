import cv2
import numpy as np
import glob

files = glob.glob("*.jpg");

oup = open('ann.txt', 'w')

for f in files:
	im = cv2.imread(f);
	if(im is not None and (im.shape[0] * im.shape[1]) > 0):
		oup.write(f + ' ' + '0 0 ' + str(im.shape[1]) + ' ' + str(im.shape[0]) + '\n')

oup.close()