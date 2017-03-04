#!/usr/bin/env python
import numpy as np
import cv2
import sys

imgName=sys.argv[1]#path for the image
img = cv2.imread(imgName)
height, width = img.shape[:2]
mask = np.zeros(img.shape[:2],np.uint8)

bgdModel = np.zeros((1,65),np.float64)
fgdModel = np.zeros((1,65),np.float64)

rect = (0,0,width,height)
cv2.grabCut(img,mask,rect,bgdModel,fgdModel,5,cv2.GC_INIT_WITH_RECT)

mask2 = np.where((mask==2)|(mask==0),0,1).astype('uint8')
img = img*mask2[:,:,np.newaxis]

cv2.imwrite('output.png',img)
# plt.imshow(img),plt.colorbar(),plt.show()
