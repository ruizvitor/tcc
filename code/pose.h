#include <cv.h>
#include <highgui.h>//import export image file

using namespace cv;

// inline void WeightedSum(matrix,i,j)
// {
//   i=;
//   j=;
//
//   // Ay=((i-1)*u)+distanceU;
//   // //Do kind of bilinear interpolation
//   // Ay= ( scores[i-1][j]*((i-2)*u) + scores[i][j]*Ay ) /
//   // ( scores[i-1][j] + scores[i][j] ); //weighted mean
//   //
//   // Ay= ( scores[i][j]*Ay + scores[i+1][j]*(i)*u ) /
//   // ( scores[i][j] + scores[i+1][j] ); //weighted mean
//   //
//   // Ax=((j-1)*v)+distanceV;
//   // Ax= ( scores[i][j-1]*((j-2)*v) + scores[i][j]*Ax ) /
//   // ( scores[i][j-1] + scores[i][j] ); //weighted mean
//   //
//   // Ax= ( scores[i][j]*Ax + scores[i][j+1]*(j)*v ) /
//   // ( scores[i][j] + scores[i][j+1] ); //weighted mean
// }

inline void Mynormalize(float *vec)
{
  float norm;
  norm=sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
  vec[0]/=norm;
  vec[1]/=norm;
  vec[2]/=norm;
}

inline void crossProduct(float *out,float *u,float *v)
{
  out[0]= (u[1]*v[2])-(u[2]*v[1]) ;
  out[1]= (u[2]*v[0])-(u[0]*v[2]) ;
  out[2]= (u[0]*v[1])-(u[1]*v[0]) ;
}

void FindPlane(  Mat &image,
                int colorR,
                int colorG,
                int colorB,
                int thresholdR,
                int thresholdG,
                int thresholdB,
                int gridsize,
                float *vecX,float *vecY,float *vecZ)
  {
    //OPENCV USES BGR NOT RGB
    // CV_BGR2HSV


    // image = imread("../material/test.JPG", CV_LOAD_IMAGE_COLOR);
    // image = imread("../material/front.JPG", CV_LOAD_IMAGE_COLOR);
    uint8_t *img = image.data;
    int n=image.rows;
    int m=image.cols;

    //TODO Change color and threshold to fuctions params
    // color(169,157,205);
    // int colorR=169;
    // int colorG=157;
    // int colorB=205;
    // int thresholdR = 50;
    // int thresholdG = 30;
    // int thresholdB = 0;

    //Pass threshold
    std::cout << "n="<< n << " m=" << m << std::endl;
    //Segment matrice histogram
    // int gridsize=10;//TODO pass as func param
    long unsigned int scores[gridsize+2][gridsize+2]; // with ghost layer
    memset(scores,0,sizeof(scores));
    int u=n/gridsize;
    int v=m/gridsize;
    std::cout << "u= " << u << " v=" << v << std::endl;
    for(int i=0;i<n;i++)
    {
      for(int j=0;j<m;j++)
      {
        if ((img[(3*((i*m)+j))]<(colorB-thresholdB) ) || (img[(3*((i*m)+j))]>(colorB+thresholdB) ) &
        (img[(3*((i*m)+j))+1]<(colorG-thresholdG) ) || (img[(3*((i*m)+j))+1]>(colorG+thresholdG) ) &
        (img[(3*((i*m)+j))+2]<(colorR-thresholdR) ) || (img[(3*((i*m)+j))+2]>(colorR+thresholdR) ))
        {
          img[(3*((i*m)+j))]=0.0;
          img[(3*((i*m)+j))+1]=0.0;
          img[(3*((i*m)+j))+2]=0.0;
        }
        else
          scores[(int)std::floor(i/u)+1][(int)std::floor(j/v)+1]++;//+1 because of ghostlayer
      }
    }
    std::cout << "threshold processed "<< std::endl;

    // //Begin Grid Lines
    // for(int i=0;i<n;i+=u)
    //   cv::line(image,Point(0,i),Point(n,i),cv::Scalar(255,255,255));
    //
    // for(int i=0;i<m;i+=v)
    //   cv::line(image,Point(i,0),Point(i,m),cv::Scalar(255,255,255));
    // //End Grid Lines

    // //Print for observation
    // for(int i=0;i<(gridsize+2);i++)
    // {
    //   for(int j=0;j<(gridsize+2);j++)
    //   {
    //     std::cout << scores[i][j] << " ";
    //   }
    //   std::cout << std::endl;
    // }

    //Create buckets
    int bucket_code=0;
    int bucket[gridsize+2][gridsize+2]; // with ghost layer
    memset(bucket,0,sizeof(bucket));
    for(int i=1;i<(gridsize+1);i++)
    {
      for(int j=1;j<(gridsize+1);j++)
      {
        if(scores[i][j]!=0)
        {
          if( bucket[i-1][j]!=0 )//N
          {
            bucket[i][j]=bucket[i-1][j];
          }
          else if( bucket[i][j-1]!=0 )//W
          {
            bucket[i][j]=bucket[i][j-1];
          }
          else if( bucket[i+1][j]!=0 )//S
          {
            bucket[i][j]=bucket[i+1][j];
          }
          else if( bucket[i][j+1]!=0 )//E
          {
            bucket[i][j]=bucket[i][j+1];
          }
          else if( bucket[i-1][j-1]!=0 )//NW
          {
            bucket[i][j]=bucket[i-1][j-1];
          }
          else if( bucket[i+1][j-1]!=0 )//SW
          {
            bucket[i][j]=bucket[i+1][j-1];
          }
          else if( bucket[i+1][j+1]!=0 )//SE
          {
            bucket[i][j]=bucket[i+1][j+1];
          }
          else if( bucket[i-1][j+1]!=0 )//NE
          {
            bucket[i][j]=bucket[i-1][j+1];
          }
          else
          {
            bucket_code++;
            bucket[i][j]=bucket_code;
          }
        }
      }
    }
    // std::cout << "bucket: " << std::endl;
    // //Print for observation
    // for(int i=0;i<(gridsize+2);i++)
    // {
    //   for(int j=0;j<(gridsize+2);j++)
    //   {
    //     std::cout << bucket[i][j] << " ";
    //   }
    //   std::cout << std::endl;
    // }

    // find the 3 most dense regions
    long unsigned int points[3]={0,0,0};
    int lastBucket=-1;
    int Ax=0;
    int Ay=0;

    int Bx=0;
    int By=0;

    int Cx=0;
    int Cy=0;

    //Find 3 most dense buckets
    for(int i=1;i<(gridsize+1);i++)
    {
      for(int j=1;j<(gridsize+1);j++)
      {
        if( (bucket[i][j]!=0) && (bucket[i][j]!=lastBucket) )
        {
          points[2]=points[1];
          Cy=By;
          Cx=Bx;
          points[1]=points[0];
          By=Ay;
          Bx=Ax;
          points[0]=0;
          lastBucket=bucket[i][j];
        }
        else if( (scores[i][j]>points[0]) )
        {
          points[0]=scores[i][j];
          Ay=i;
          Ax=j;
        }
      }
    }

    // int distanceU = u/2;
    // int distanceV = v/2;
    // WeightedSum(scores,Ay,Ax);
    // WeightedSum(scores,By,Bx);
    // WeightedSum(scores,Cy,Cx);

    //Plot for test
    // circle(image,Point(Ax,Ay),3,Scalar(255,0,0));
    // circle(image,Point(Bx,By),3,Scalar(255,0,0));
    // circle(image,Point(Cx,Cy),3,Scalar(255,0,0));
    //Print points
    // std::cout << points[0] << " " << points[1] << " " << points[2] << std::endl;

    //Calculate euclidian distance between points
    //A, B, C  AyAx , ByBx , CyCx
    float AB = sqrt( (Ay-By)*(Ay-By) + (Ax-Bx)*(Ax-Bx) );
    float AC = sqrt( (Ay-Cy)*(Ay-Cy) + (Ax-Cx)*(Ax-Cx) );
    float BC = sqrt( (By-Cy)*(By-Cy) + (Bx-Cx)*(Bx-Cx) );
    float comb1=AB+AC;
    float comb2=AB+BC;
    float comb3=AC+BC;
    //The point with the smallest sum is the new origin
    if( (comb1 < comb2) & (comb1 < comb3) )
    {
      //New Origin A
      //Translate points to Origin
      if( abs(Ax-Bx)>abs(Ax-Cx) )
      {
        //B new vecX
        vecX[0]=Bx;
        vecX[2]=By;
        //C new vecZ
        vecZ[0]=Cx;
        vecZ[2]=Cy;
      }
      else
      {
        //C new vecX
        vecX[0]=Cx;
        vecX[2]=Cy;
        //B new vecZ
        vecZ[0]=Bx;
        vecZ[2]=By;
      }
      vecX[0]-=Ax;
      vecX[2]-=Ay;

      vecZ[0]-=Ax;
      vecZ[2]-=Ay;
    }
    else if( (comb2 < comb1) & (comb2 < comb3) )
    {
      //New Origin B
      //Translate points to Origin
      if( abs(Bx-Ax)>abs(Bx-Cx) )
      {
        //A new vecX
        vecX[0]=Ax;
        vecX[2]=Ay;
        //C new vecZ
        vecZ[0]=Cx;
        vecZ[2]=Cy;
      }
      else
      {
        //C new vecX
        vecX[0]=Cx;
        vecX[2]=Cy;
        //A new vecZ
        vecZ[0]=Ax;
        vecZ[2]=Ay;
      }
      vecX[0]-=Bx;
      vecX[2]-=By;

      vecZ[0]-=Bx;
      vecZ[2]-=By;
    }
    else if( (comb3 < comb1) & (comb3 < comb2) )
    {
      //New Origin B
      //Translate points to Origin
      if( abs(Cx-Bx)>abs(Cx-Ax) )
      {
        //B new vecX
        vecX[0]=Bx;
        vecX[2]=By;
        //A new vecZ
        vecZ[0]=Ax;
        vecZ[2]=Ay;
      }
      else
      {
        //A new vecX
        vecX[0]=Ax;
        vecX[2]=Ay;
        //B new vecZ
        vecZ[0]=Bx;
        vecZ[2]=By;
      }

      vecX[0]-=Cx;
      vecX[2]-=Cy;

      vecZ[0]-=Cx;
      vecZ[2]-=Cy;
    }
    vecX[1]=0.0;
    vecZ[1]=0.0;
    //Calculate vectors
    Mynormalize(vecX);
    Mynormalize(vecY);
    Mynormalize(vecZ);
    crossProduct(vecY,vecX,vecZ);//OUT IN IN

    std::cout << "{ " << vecX[0] << " , " << vecX[1] << " , " << vecX[2] << " }" << std::endl;
    std::cout << "{ " << vecY[0] << " , " << vecY[1] << " , " << vecY[2] << " }" << std::endl;
    std::cout << "{ " << vecZ[0] << " , " << vecZ[1] << " , " << vecZ[2] << " }" << std::endl;
    // imwrite("out/result.bmp",image);
  }
