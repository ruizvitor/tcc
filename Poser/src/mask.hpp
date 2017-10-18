#ifndef _MASK_H_
#define _MASK_H_

// #include <cv.h>
#include <opencv2/opencv.hpp>
using namespace cv;

void Erosion( Mat &src );

Mat makeMask(Mat src_gray);

void ApplyMask(Mat M, std::vector< unsigned char >& frameImage);


#endif // _MASK_H_
