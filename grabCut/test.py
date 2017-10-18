#!/usr/bin/env python
from __future__ import division
import cv2
import sys
import numpy as np
# import imutils

maskName=sys.argv[1]#path for the image
imgName=sys.argv[2]#path for the image

#Read File
mymask = cv2.imread(maskName,cv2.IMREAD_GRAYSCALE)#IN BGR
mymask = cv2.resize(mymask, (0,0), fx=8, fy=8)
# mymask = cv2.GaussianBlur(mymask,(15,15),0)
img = cv2.imread(imgName,cv2.IMREAD_COLOR)#IN BGR

output = cv2.bitwise_and(img,img,mask=mymask)

# #Resize Operation
# h, w = img.shape[:2]

# cv2.imshow("Image", output)
# key = cv2.waitKey(0)
cv2.imwrite(imgName+"b.JPG", output);
