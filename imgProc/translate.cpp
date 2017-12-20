#include <stdlib.h>
#include <unistd.h>
#include <cv.h>
#include <highgui.h>
#include <iostream>

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
  Mat image;
  image=imread(argv[1],CV_LOAD_IMAGE_COLOR);
  // uint8_t *img = image.data;
  int n=image.rows;
  int m=image.cols;

  int centerx=atoi(argv[2]);
  int centery=atoi(argv[3]);

  cout << "centerx_act="<< centerx << " centery_act=" << centery << endl;

  Mat warpGround;
  warpGround = (Mat_<float>(2,3) << 1, 0, centerx,
                                    0, 1, centery);
  warpAffine(image, image, warpGround,
            Size(m,n), INTER_LINEAR);

  imwrite(argv[1],image);

  return 0;
}
