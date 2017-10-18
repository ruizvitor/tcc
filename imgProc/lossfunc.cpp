
#include <stdlib.h>
#include <unistd.h>
#include <cv.h>
#include <highgui.h>
#include <iostream>
#include "../code/lossfunc.h"

using namespace cv;
using namespace std;

double Loss2(cv::Mat &image, cv::Mat &Mm,  double Favg, double Cff)
{
  // double Cmm[3][3];
  std::vector< double > Cmm(9,0);
  std::vector< double > Cfm(3,0);
  std::vector< double > tempvec(3,0);
  unsigned long int Msumx=0;
  unsigned long int Msumy=0;
  unsigned long int Msumz=0;
  double Mavgx=0;
  double Mavgy=0;
  double Mavgz=0;

  uint8_t *F = image.data;
  uint8_t *M = Mm.data;
  unsigned int n=image.rows;
  unsigned int m=image.cols;
  unsigned long int size=n*m;
  int address=0;
  // //Mean
  // for(unsigned i=0;i<n;i++)
  // {
  //   for(unsigned j=0;j<m;j++)
  //   {
  //     Fsum+=F.buf[i*m+j];
  //   }
  // }
  // cout << "Fsum: "<< Fsum << endl;
  // Favg=Fsum/size;
  // cout << "Favg: "<< Favg << endl;
  //
  // cout << "Color: "<< M[0] << "Color: "<< M[1] << "Color: "<< M[2]  << endl;
  //MeanVec3
  for(unsigned i=0;i<n;i++)
  {
    for(unsigned j=0;j<m;j++)
    {
      address=(3*((i*m)+j));
      Msumx+=M[address];
      Msumy+=M[address+1];
      Msumz+=M[address+2];
    }
  }
  // cout << "Msum.x: "<< Msumx << " Msumy: "<< Msumy << " Msumz: "<< Msumz << endl;
  Mavgx=Msumx/size;
  Mavgy=Msumy/size;
  Mavgz=Msumz/size;
  // cout << "Mavgx: "<< Mavgx << " Mavgy: "<< Mavgy << " Mavgz: "<< Mavgz << endl;
  //Cff
  // for(unsigned i=0;i<n;i++)
  // {
  //   for(unsigned j=0;j<m;j++)
  //   {
  //     Cff+=(F.buf[i*m+j]-Favg)*(F.buf[i*m+j]-Favg);
  //   }
  // }
  // Cff=Cff/size;
  // cout << "Cff: "<< Cff << endl;
  //Calculate Cfm
  double Cfmsumx=0.0;
  double Cfmsumy=0.0;
  double Cfmsumz=0.0;
  for(unsigned i=0;i<n;i++)
  {
    for(unsigned j=0;j<m;j++)
    {
      Cfmsumx+=(F[i*m+j]-Favg) * (M[(3*((i*m)+j))]- Mavgx);
      Cfmsumy+=(F[i*m+j]-Favg) * (M[(3*((i*m)+j))+1]- Mavgy);
      Cfmsumz+=(F[i*m+j]-Favg) * (M[(3*((i*m)+j))+2]- Mavgz);
    }
  }

  Cfm[0]=Cfmsumx/size;
  Cfm[1]=Cfmsumy/size;
  Cfm[2]=Cfmsumz/size;
  // cout << "Cfm.x: "<< Cfm[0] << " Cfm.y: "<< Cfm[1] << " Cfm.z: "<< Cfm[2] << endl;

  // cout << "invCff: " << Cff << endl;

  double tempx=0.0;
  double tempy=0.0;
  double tempz=0.0;
  for(unsigned i=0;i<n;i++)
  {
    for(unsigned j=0;j<m;j++)
    {
      tempx=M[(3*((i*m)+j))]- Mavgx;
      tempy=M[(3*((i*m)+j))+1]- Mavgy;
      tempz=M[(3*((i*m)+j))+2]- Mavgz;

      Cmm[0]+=tempx*tempx;
      Cmm[1]+=tempx*tempy;
      Cmm[2]+=tempx*tempz;

      Cmm[3]+=tempy*tempx;
      Cmm[4]+=tempy*tempy;
      Cmm[5]+=tempy*tempz;

      Cmm[6]+=tempz*tempx;
      Cmm[7]+=tempz*tempy;
      Cmm[8]+=tempz*tempz;
    }
  }

  //
  // //j i
  // // Coovariance Matrix
  // tempx/=size;
  // tempy/=size;
  // tempz/=size;
  // cout << "tempx: "<< tempx << " tempy: "<< tempy << " tempz: "<< tempz << endl;

  Cmm[0]/=size;
  Cmm[1]/=size;
  Cmm[2]/=size;

  Cmm[3]/=size;
  Cmm[4]/=size;
  Cmm[5]/=size;

  Cmm[6]/=size;
  Cmm[7]/=size;
  Cmm[8]/=size;

  // cout << "[0][0]: "<< Cmm[0]  << "[0][1]: "<< Cmm[1]  << "[0][2]: "<< Cmm[2] << endl;
  // cout << "[1][0]: "<< Cmm[3]  << "[1][1]: "<< Cmm[4]  << "[1][2]: "<< Cmm[5] << endl;
  // cout << "[2][0]: "<< Cmm[6]  << "[2][1]: "<< Cmm[7]  << "[2][2]: "<< Cmm[8] << endl;
  //Inverse Matrix
  // std::vector< double > inv_Cmm(9);
  // cv::invert(Cmm,inv_Cmm,cv::DECOMP_LU);
  Cmm=matinverse(Cmm);
  // cout << "[0][0]: "<< Cmm[0]  << "[0][1]: "<< Cmm[1]  << "[0][2]: "<< Cmm[2] << endl;
  // cout << "[1][0]: "<< Cmm[3]  << "[1][1]: "<< Cmm[4]  << "[1][2]: "<< Cmm[5] << endl;
  // cout << "[2][0]: "<< Cmm[6]  << "[2][1]: "<< Cmm[7]  << "[2][2]: "<< Cmm[8] << endl;
  // cout << "[0][0]: "<< inv_Cmm[0]  << "[0][1]: "<< inv_Cmm[1]  << "[0][2]: "<< inv_Cmm[2] << endl;
  // cout << "[1][0]: "<< inv_Cmm[3]  << "[1][1]: "<< inv_Cmm[4]  << "[1][2]: "<< inv_Cmm[5] << endl;
  // cout << "[2][0]: "<< inv_Cmm[6]  << "[2][1]: "<< inv_Cmm[7]  << "[2][2]: "<< inv_Cmm[8] << endl;


  tempvec[0]=Cfm[0]*Cmm[0] + Cfm[1]*Cmm[3] + Cfm[2]*Cmm[6];
  tempvec[1]=Cfm[0]*Cmm[1] + Cfm[1]*Cmm[4] + Cfm[2]*Cmm[7];
  tempvec[2]=Cfm[0]*Cmm[2] + Cfm[1]*Cmm[5] + Cfm[2]*Cmm[8];

  double final= 1-((tempvec[0]*Cfm[0] + tempvec[1]*Cfm[1] + tempvec[2]*Cfm[2])*Cff);
  // double final= ((tempvec[0]*Cfm[0] + tempvec[1]*Cfm[1] + tempvec[2]*Cfm[2])*Cff);

  return final;
  // return 1;
}

int main( int argc, char** argv )
{
  //Default
  //Images


  Mat imageGray = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
  //imageColor = imread("../material/testes/back.JPG", CV_LOAD_IMAGE_COLOR);
  //toGray(imageColor,imageGray);//use average desaturate instead of lightness
  //int height=imageGray.rows;
  //int width=imageGray.cols;

  Mat frame = imread(argv[2], CV_LOAD_IMAGE_COLOR);
  std::vector< unsigned char > frameImage(frame.begin<unsigned char>(), frame.end<unsigned char>());

  float favg=Mean(imageGray);
  float cff=MyCff(imageGray,favg);

  float distance=Loss2(imageGray, frame, favg,cff);
  cout<< "distance="<< distance << endl;
}
