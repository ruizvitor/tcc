typedef struct{
  int x;
  int y;
  int centerx;
  int centery;
}point;

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

  diff.x=xmax;
  diff.y=ymax;
  diff.centerx=xmin+(xmax-xmin)/2;
  diff.centery=ymin+(ymax-ymin)/2;
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

  diff.x=xmax;
  diff.y=ymax;
  diff.centerx=xmin+(xmax-xmin)/2;
  diff.centery=ymin+(ymax-ymin)/2;
}
