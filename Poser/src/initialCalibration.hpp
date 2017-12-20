#ifndef _CALI_H_
#define _CALI_H_

#include <iostream>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;

typedef struct{
  int x;
  int y;
  int xmin;
  int ymin;
  int centerx;
  int centery;
}point;

void getInitPose(glm::mat4& initialRot,const char* optarg);

void getBetterInitPose(float &near,  glm::mat4& T,glm::mat4& R, glm::mat4& S,int& tx, int& ty,const char* optarg);

void getBoundingBox(Mat& image, point& diff);

void getBoundingBoxColor(std::vector< unsigned char >& img, point& diff, int n, int m);

#endif // _CALI_H_
