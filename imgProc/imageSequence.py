#!/usr/bin/env python
# import cv2
# import sys
#
# #Then, to open the video file:
#
# vidName=sys.argv[1]#path for the video
# print vidName
#
# vidcap = cv2.VideoCapture(vidName)
#
# #The easiest way to extract frames is to use the read() method on the vidcap object. It returns a tuple where the first element is a success flag and the second is the image array. To extract the image array:
#
# success,image = vidcap.read()
# # image is an array of array of [R,G,B] values
#
# #To convert the whole video to frames and save each one:
#
# print success
# count = 0;
# while success:
#     success,image = vidcap.read()
#     cv2.imwrite("frames/frame%d.jpg" % count, image)     # save frame as JPEG file
#     if cv2.waitKey(10) == 27:                     # exit if Escape is hit
#         break
#     count += 1

import cv2
import sys
import skvideo.io
cap = skvideo.io.VideoCapture(sys.argv[1])
success, image = vidcap.read()

#The easiest way to extract frames is to use the read() method on the vidcap object. It returns a tuple where the first element is a success flag and the second is the image array. To extract the image array:

# success,image = vidcap.read()
# image is an array of array of [R,G,B] values

#To convert the whole video to frames and save each one:

print success
count = 0;
while success:
    success,image = vidcap.read()
    cv2.imwrite("frames/frame%d.jpg" % count, image)     # save frame as JPEG file
    if cv2.waitKey(10) == 27:                     # exit if Escape is hit
        break
    count += 1
