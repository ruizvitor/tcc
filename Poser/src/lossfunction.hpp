#ifndef _LOSSFUNC_H_
#define _LOSSFUNC_H_

// #include <cv.h>
#include <opencv2/opencv.hpp>

double Mean(cv::Mat &image);

double MyCff(cv::Mat &image,double Favg);

std::vector< double > matinverse(std::vector< double > &A);

double Loss(cv::Mat &image, std::vector< unsigned char >&M,  double Favg, double Cff);

#endif // _LOSSFUNC_H_
