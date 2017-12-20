#include "initialCalibration.hpp"

void getInitPose(glm::mat4& initialRot,const char* optarg)
{
  FILE* fpose=fopen(optarg, "r" );
  if (fpose!=NULL)
  {
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        fscanf(fpose,"%f",&initialRot[i][j]);
        // std::cout << "initialRot["<<i<<"]["<<j<<"]="<<initialRot[i][j] << std::endl;
      }
    }
    // initialRot[1][1]=88.0f;
    std::cout << "sucess in reading pose file" << std::endl;
    fclose (fpose);
  }
  else
  {
    std::cout << "fail to open file with read privilage" << std::endl;
    abort();
  }
}

void getBetterInitPose(float &near,  glm::mat4& T,glm::mat4& R, glm::mat4& S,int& tx, int& ty,const char* optarg)
{
  FILE* fpose=fopen(optarg, "r" );
  if (fpose!=NULL)
  {
    fscanf(fpose,"%f",&near);
    std::cout<<near<<std::endl;
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        fscanf(fpose,"%f",&T[i][j]);
        std::cout<<T[i][j]<<" ";
      }
      std::cout<<std::endl;
    }
    std::cout<<std::endl;
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        fscanf(fpose,"%f",&R[i][j]);
        std::cout<<R[i][j]<<" ";
      }
      std::cout<<std::endl;
    }
    std::cout<<std::endl;
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        fscanf(fpose,"%f",&S[i][j]);
        std::cout<<S[i][j]<<" ";
      }
      std::cout<<std::endl;
    }
    std::cout<<std::endl;
    fscanf(fpose,"%d",&tx);
    std::cout<<tx<<" ";
    fscanf(fpose,"%d",&ty);
    std::cout<<ty<<std::endl;
    std::cout << "sucess in reading pose file" << std::endl;
    fclose (fpose);
  }
  else
  {
    std::cout << "fail to open file with read privilage" << std::endl;
    abort();
  }
}

void getBoundingBox(Mat& image, point& diff)
{
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
      if( (img[(i*m+j)]!=0) )
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
  // cout<<"xmin="<<xmin<<" ymin="<<ymin<<endl;
  // cout<<"xmax="<<xmax<<" ymax="<<ymax<<endl;
  // cout<<"diffx="<<xmax-xmin<<" diffy="<<ymax-ymin<<endl;

  diff.xmin=xmin;
  diff.ymin=ymin;
  diff.x=xmax;
  diff.y=ymax;
  // diff.x=0.99*xmax;
  // diff.y=0.99*ymax;
  diff.centerx=xmin+((xmax-xmin)/2);
  diff.centery=ymin+((ymax-ymin)/2);
  // std::cout<<"xmin+((xmax-xmin)/2)=="<<diff.centerx<<" ymin+((ymax-ymin)/2)=="<<diff.centery<<std::endl;
}

void getBoundingBoxColor(std::vector< unsigned char >& img, point& diff, int n, int m)
{
  // uint8_t *img = image.data;
  // int n=image.rows;
  // int m=image.cols;

  int xmin=m;
  int xmax=0;
  int ymin=n;
  int ymax=0;

  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < m; j++)
    {
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
  // cout<<"xmin="<<xmin<<" ymin="<<ymin<<endl;
  // cout<<"xmax="<<xmax<<" ymax="<<ymax<<endl;
  // cout<<"diffx="<<xmax-xmin<<" diffy="<<ymax-ymin<<endl;

  diff.xmin=xmin;
  diff.ymin=ymin;
  diff.x=xmax;
  diff.y=ymax;
  diff.centerx=xmin+(xmax-xmin)/2;
  diff.centery=ymin+(ymax-ymin)/2;
}
