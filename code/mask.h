#ifndef _MASK_H_
#define _MASK_H_

using namespace cv;

void Erosion( Mat &src )
{
  // int erosion_size = 15;
  int erosion_size = 60;
  cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS,
                        cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
                        cv::Point(erosion_size, erosion_size) );

  erode( src, src, element );
}

Mat makeMask(Mat src_gray)
{
  Mat mask;
  threshold( src_gray, mask, 1, 255, CV_THRESH_BINARY );
  // Erosion(mask);
  return mask;
}

void ApplyMask(Mat M, std::vector< unsigned char >& frameImage)
{

  uint8_t *mask = M.data;
  unsigned int n=M.rows;
  unsigned int m=M.cols;

  for(unsigned i=0;i<n;i++)
  {
    for(unsigned j=0;j<m;j++)
    {
      int address=(3*((i*m)+j));
      if(mask[(i*m)+j]==0)
      {
        frameImage[address]=0;
        frameImage[address+1]=0;
        frameImage[address+2]=0;
      }

    }
  }

}

#endif // _MASK_H_
