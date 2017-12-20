#!/usr/bin/env python
from __future__ import division
import cv2
import sys
import numpy as np
# import imutils

maskName=sys.argv[1]#path for the image
imgName=sys.argv[2]#path for the image
# resize=sys.argv[3]#path for the image
# print "resize=",resize


#Read File
mymask = cv2.imread(maskName,cv2.IMREAD_GRAYSCALE)#IN BGR
th, mymask = cv2.threshold(mymask,1,255,cv2.THRESH_BINARY)
kernel = np.ones((5,5),np.uint8)
# mymask = cv2.erode(mymask,kernel,iterations = 1)
# mymask = cv2.imread(maskName,cv2.IMREAD_COLOR)#IN BGR
# mymask = cv2.GaussianBlur(mymask,(5,5),0)
img = cv2.imread(imgName,cv2.IMREAD_COLOR)#IN BGR
h, w = img.shape[:2]
h2, w2 = mymask.shape[:2]
# mymask = cv2.GaussianBlur(mymask,(int(h2*0.1),int(w2*0.1)),0)
if((h!=h2)or(w!=w2)):
    mymask = cv2.resize(mymask, (0,0), fx=4, fy=4)
    # mymask = cv2.erode(mymask,kernel,iterations = 5)
output = cv2.bitwise_and(img,img,mask=mymask)

# #Resize Operation
# h, w = img.shape[:2]

# cv2.imshow("Image", output)
# key = cv2.waitKey(0)
cv2.imwrite(imgName+"b.JPG", output);
