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
  uint8_t *img = image.data;
  int n=image.rows;
  int m=image.cols;

  int xmin=m;
  int xmax=0;
  int ymin=n;
  int ymax=0;

  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < m; j++)
    {
      //yellow
      if(
      (img[(i*m*3+j*3)+2]!=0)||
      (img[(i*m*3+j*3)+1]!=0)||
      (img[(i*m*3+j*3)]!=0) )
      {
        if(j<xmin)
            xmin=j;
        if(j>xmax)
            xmax=j;
        if(i<ymin)
            ymin=i;
        if(i>ymax)
            ymax=i;
      }
    }
  }

  cout << "diffx=" << (xmax-xmin) << " diffy=" << (ymax-ymin) << endl;
  int centerx=xmin+(xmax-xmin)/2;
  int centery=ymin+(ymax-ymin)/2;
  cout << "centerx_act="<< centerx << " centery_act=" << centery << endl;
  cout << "centerx_real="<< m/2 << " centery_real=" << n/2 << endl;
  if((centerx==m/2)&&(centery==n/2)){
    return 0;  
  }
  centerx=(m/2)-centerx;
  centery=(n/2)-centery;

  Mat warpGround;
  warpGround = (Mat_<float>(2,3) << 1, 0, centerx,
                                    0, 1, centery);
  warpAffine(image, image, warpGround,
            Size(m,n), INTER_LINEAR);  
  //warpAffine(image, image, warpGround,
  //          Size(m,n), INTER_LINEAR);
// warpAffine(image, image, warpGround);

  // warpAffine(img,img,translate);

  // imshow("Image",image);
  // waitKey(0);
  imwrite(argv[1],image);

  return 0;
}
