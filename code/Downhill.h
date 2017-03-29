#include <stdlib.h>
#define WINDOW_HEIGHT 1024
#define WINDOW_WIDTH 1024
#define MAX_ITER 300
#define MIN_ERROR 0.01
#define MIN_VAR 0.00001
#define N_DIMENSIONS 7
#define N_DIMENSIONSSUM 8

float specialRand()
{
  return ( (rand()%2) ? ((float)rand()/(float)(RAND_MAX)) : (-1.0*(float)rand()/(float)(RAND_MAX)) ) ;
}

void CopyArray(float B[],float A[],int n,float* placeholder, float* x,int m)
{
    for(int i = 0; i < n; i++)
    {
        A[i] = B[i];
        for(int j=0;j<m;j++)
        {
          x[(i*m)+j] = placeholder[(i*m)+j];
        }
    }
}

//  Left run is A[iLeft :iRight-1].
// Right run is A[iRight:iEnd-1  ].
void BottomUpMerge(float A[],int iLeft,int iRight,int iEnd,float B[],float* placeholder, float* x,int m)
{
    int i = iLeft;
    int j = iRight;
    // While there are elements in the left or right runs...
    for (int k = iLeft; k < iEnd; k++)
    {
        // If left run head exists and is <= existing right run head.
        if (i < iRight && (j >= iEnd || A[i] <= A[j]))
        {
            B[k] = A[i];
            for(int w=0;w<m;w++)
            {
              placeholder[(k*m)+w] = x[(i*m)+w] ;
            }
            i = i + 1;
        }
        else
        {
            B[k] = A[j];
            for(int w=0;w<m;w++)
            {
              placeholder[(k*m)+w] = x[(j*m)+w] ;
            }
            j = j + 1;
        }
    }
}




// array A[] has the items to sort; array B[] is a work array
void BottomUpMergeSort(float A[],int n, float* x,int m)
{
    float B[n];
    float placeholder[n*m];
    // Each 1-element run in A is already "sorted".
    // Make successively longer sorted runs of length 2, 4, 8, 16... until whole array is sorted.
    for (int width = 1; width < n; width = 2 * width)
    {
        // Array A is full of runs of length width.
        for (int i = 0; i < n; i = i + 2 * width)
        {
            // Merge two runs: A[i:i+width-1] and A[i+width:i+2*width-1] to B[]
            // or copy A[i:n-1] to B[] ( if(i+width >= n) )
            BottomUpMerge(A, i, min(i+width, n), min(i+2*width, n), B, placeholder, x, m);
        }
        // Now work array B is full of runs of length 2*width.
        // Copy array B to array A for next iteration.
        // A more efficient implementation would swap the roles of A and B.
        CopyArray(B, A, n, placeholder, x, m);
        // Now array A is full of runs of length 2*width.
    }
}


// void specialSort()
// {
//   for(int i=0;i<;i++)
//   {
//     if(value)
//     {
//
//     }
//   }
// }

void insertionFancy(float* fx,float value, float *face, float* x)
{
  int j=-1;

  for(int i=0;i<(N_DIMENSIONSSUM);i++)
  {
    // cout << "value:"<< value << " fx[" << i << "]:" << fx[i] << endl;
    if(value<fx[i])
    {
      for(int j=N_DIMENSIONS;j>i;j--)
      {
        fx[j]=fx[j-1];

        x[(j*N_DIMENSIONS)]=x[((j-1)*N_DIMENSIONS)];
        x[(j*N_DIMENSIONS)+1]=x[((j-1)*N_DIMENSIONS)+1];
        x[(j*N_DIMENSIONS)+2]=x[((j-1)*N_DIMENSIONS)+2];

        x[(j*N_DIMENSIONS)+3]=x[((j-1)*N_DIMENSIONS)+3];
        x[(j*N_DIMENSIONS)+4]=x[((j-1)*N_DIMENSIONS)+4];
        x[(j*N_DIMENSIONS)+5]=x[((j-1)*N_DIMENSIONS)+5];

        x[(j*N_DIMENSIONS)+6]=x[((j-1)*N_DIMENSIONS)+6];
      }
      fx[i]=value;
      j=i;
      break;
    }
  }

  // cout << "\nfx[0]:" << fx[0]
  // << " fx[1]:" << fx[1]
  // << " fx[2]:" << fx[2]
  // << " fx[6]:" << fx[6] << endl;

  if(j!=-1)
  {
    x[(j*N_DIMENSIONS)]=face[0];
    x[(j*N_DIMENSIONS)+1]=face[1];
    x[(j*N_DIMENSIONS)+2]=face[2];

    x[(j*N_DIMENSIONS)+3]=face[3];
    x[(j*N_DIMENSIONS)+4]=face[4];
    x[(j*N_DIMENSIONS)+5]=face[5];

    x[(j*N_DIMENSIONS)+6]=face[6];
  }
}

void downhill(float* fw,
              float* w,
              GLuint myFrameBuffer,
              Shader & simpleShader,
              Model & ourModel,
              glm::mat4 & M,
              glm::mat4 & V,
              glm::mat4 & P,
              double favg,
              double cff,
              Mat & imageGray
            )
{
  //Init Variables
  std::vector< unsigned char > frameImage;
  float alfa=1.0;
  float beta=1.0;
  float gama=0.5;
  float sigma=0.5;
  float distance=1.0;
  float distance_reflec=1.0;
  float distance_expan=1.0;
  float distance_contrac=1.0;
  float distance_shrink=1.0;
  float last_distance=1.0;
  float var_distance=1.0;
  float rotVecNew[16] = {
     1.0f, 0.0f, 0.0f, 0.0f,
     0.0f, 1.0f, 0.0f, 0.0f,
     0.0f, 0.0f, 1.0f, 0.0f,
     0.0f, 0.0f, 0.0f, 1.0f
  };
  glm::mat4 rotMat;
  float reflect[N_DIMENSIONS];
  float expanded[N_DIMENSIONS];
  float contracted[N_DIMENSIONS];
  float shrink[N_DIMENSIONS];
  float centroid[N_DIMENSIONS];
  for(int i=0;(i<MAX_ITER)&&(distance>MIN_ERROR)&&(fabs(var_distance)>MIN_VAR);i++)//&&(distance>MIN_ERROR)&&(fabs(var_distance)>MIN_ERROR)
  {
    //Calculate centroid

    // centroid[0]= (w[0]+w[6]+w[12]+w[18]+w[24]+w[30])/6;
    // centroid[1]= (w[1]+w[7]+w[13]+w[19]+w[25]+w[31])/6;
    // centroid[2]= (w[2]+w[8]+w[14]+w[20]+w[26]+w[32])/6;
    //
    // centroid[3]= (w[3]+w[9]+w[15]+w[21]+w[27]+w[33])/6;
    // centroid[4]= (w[4]+w[10]+w[16]+w[22]+w[28]+w[34])/6;
    // centroid[5]= (w[5]+w[11]+w[17]+w[23]+w[29]+w[35])/6;

    centroid[0]= (w[0]+w[N_DIMENSIONS]+w[(N_DIMENSIONS*2)]+w[(N_DIMENSIONS*3)]+w[(N_DIMENSIONS*4)]+w[(N_DIMENSIONS*5)]+w[(N_DIMENSIONS*6)])/7;
    centroid[1]= (w[1]+w[N_DIMENSIONS+1]+w[(N_DIMENSIONS*2)+1]+w[(N_DIMENSIONS*3)+1]+w[(N_DIMENSIONS*4)+1]+w[(N_DIMENSIONS*5)+1]+w[(N_DIMENSIONS*6)+1])/7;
    centroid[2]= (w[2]+w[N_DIMENSIONS+2]+w[(N_DIMENSIONS*2)+2]+w[(N_DIMENSIONS*3)+2]+w[(N_DIMENSIONS*4)+2]+w[(N_DIMENSIONS*5)+2]+w[(N_DIMENSIONS*6)+2])/7;

    centroid[3]= (w[3]+w[N_DIMENSIONS+3]+w[(N_DIMENSIONS*2)+3]+w[(N_DIMENSIONS*3)+3]+w[(N_DIMENSIONS*4)+3]+w[(N_DIMENSIONS*5)+3]+w[(N_DIMENSIONS*6)+3])/7;
    centroid[4]= (w[4]+w[N_DIMENSIONS+4]+w[(N_DIMENSIONS*2)+4]+w[(N_DIMENSIONS*3)+4]+w[(N_DIMENSIONS*4)+4]+w[(N_DIMENSIONS*5)+4]+w[(N_DIMENSIONS*6)+4])/7;
    centroid[5]= (w[5]+w[N_DIMENSIONS+5]+w[(N_DIMENSIONS*2)+5]+w[(N_DIMENSIONS*3)+5]+w[(N_DIMENSIONS*4)+5]+w[(N_DIMENSIONS*5)+5]+w[(N_DIMENSIONS*6)+5])/7;

    centroid[6]= (w[6]+w[N_DIMENSIONS+6]+w[(N_DIMENSIONS*2)+6]+w[(N_DIMENSIONS*3)+6]+w[(N_DIMENSIONS*4)+6]+w[(N_DIMENSIONS*5)+6]+w[(N_DIMENSIONS*6)+6])/7;
    // cout << centroid[0] << " " << centroid[1] << " " << centroid[2] << endl;
    // cout << centroid[3] << " " << centroid[4] << " " << centroid[5] << endl << endl;
    //Reflection

    reflect[0] = centroid[0] + alfa*(centroid[0]-w[(N_DIMENSIONS*N_DIMENSIONS)]);
    reflect[1] = centroid[1] + alfa*(centroid[1]-w[(N_DIMENSIONS*N_DIMENSIONS)+1]);
    reflect[2] = centroid[2] + alfa*(centroid[2]-w[(N_DIMENSIONS*N_DIMENSIONS)+2]);

    reflect[3] = centroid[3] + alfa*(centroid[3]-w[(N_DIMENSIONS*N_DIMENSIONS)+3]);
    reflect[4] = centroid[4] + alfa*(centroid[4]-w[(N_DIMENSIONS*N_DIMENSIONS)+4]);
    reflect[5] = centroid[5] + alfa*(centroid[5]-w[(N_DIMENSIONS*N_DIMENSIONS)+5]);

    reflect[6] = centroid[6] + alfa*(centroid[6]-w[(N_DIMENSIONS*N_DIMENSIONS)+6]);
    float normX=sqrt(reflect[0]*reflect[0]+
                    reflect[1]*reflect[1]+
                    reflect[2]*reflect[2]);

    float normY=sqrt(reflect[3]*reflect[3]+
                    reflect[4]*reflect[4]+
                    reflect[5]*reflect[5]);
    reflect[0] = reflect[0]/normX;
    reflect[1] = reflect[1]/normX;
    reflect[2] = reflect[2]/normX;

    reflect[3] = reflect[3]/normY;
    reflect[4] = reflect[4]/normY;
    reflect[5] = reflect[5]/normY;

    // cout << "{" << reflect[0] << "," << reflect[1] << "," << reflect[2] << "} ";
    // cout << "{" << reflect[3] << "," << reflect[4] << "," << reflect[5] << "} ";
    //Bind FrameBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
    /* Render here */
    // Clear the colorbuffer
    glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    simpleShader.Use();

    //Transformations (Translate*Rotate*Scale)
    M = glm::mat4(1);//Init

    glm::vec3 myZ=cross(glm::vec3(reflect[0],reflect[1],reflect[2]),glm::vec3(reflect[3],reflect[4],reflect[5]) );
    // cout << "{" << myZ[0] << "," << myZ[1] << "," << myZ[2] << "} " << endl << endl;

    rotVecNew[0]=reflect[0];//x1
    rotVecNew[1]=reflect[3];//y1
    rotVecNew[2]=myZ[0];//z1

    rotVecNew[4]=reflect[1];//x2
    rotVecNew[5]=reflect[4];//y2
    rotVecNew[6]=myZ[1];//z2

    rotVecNew[8]=reflect[2];//x3
    rotVecNew[9]=reflect[5];//y3
    rotVecNew[10]=myZ[2];//z3

    memcpy( glm::value_ptr( rotMat ), rotVecNew, sizeof( rotVecNew ) );
    M = M * rotMat;
    M = glm::scale(M, glm::vec3(reflect[6],reflect[6],reflect[6]) );//Scale

    simpleShader.BindMatrices(&M,&V,&P);

    ourModel.Draw(simpleShader);
    // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 80);
    frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
    distance=Loss(imageGray, frameImage, favg,cff);
    var_distance=last_distance-distance;
    last_distance=distance;
    distance_reflec=distance;

    cout << "\ndownhill calcule reflect: distance:" <<distance << " var_distance: " << var_distance << endl;
    // cout<< endl;
    // for(int i=0;i<7;i++)
    // {
    //   cout << fw[i]<< " ";
    // }
    // cout<< endl;
    /* unbind frameBuffer */
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Check Reflection
    if((distance_reflec>fw[0])&&(distance_reflec<fw[N_DIMENSIONS-1]))//f(x1)<f(xr)<f(xn)
    {
      insertionFancy(fw,distance_reflec,reflect,w);
    }

    //Expansion
    if(distance_reflec<fw[0])//f(xr)<f(x1)
    {
      //Do expansion
      //Calculate expanded

      expanded[0]=reflect[0]+beta*(reflect[0]-centroid[0]);
      expanded[1]=reflect[1]+beta*(reflect[1]-centroid[1]);
      expanded[2]=reflect[2]+beta*(reflect[2]-centroid[2]);

      expanded[3]=reflect[3]+beta*(reflect[3]-centroid[3]);
      expanded[4]=reflect[4]+beta*(reflect[4]-centroid[4]);
      expanded[5]=reflect[5]+beta*(reflect[5]-centroid[5]);

      expanded[6]=reflect[6]+beta*(reflect[6]-centroid[6]);
      normX=sqrt(expanded[0]*expanded[0]+
                      expanded[1]*expanded[1]+
                      expanded[2]*expanded[2]);

      normY=sqrt(expanded[3]*expanded[3]+
                      expanded[4]*expanded[4]+
                      expanded[5]*expanded[5]);
      expanded[0] = expanded[0]/normX;
      expanded[1] = expanded[1]/normX;
      expanded[2] = expanded[2]/normX;

      expanded[3] = expanded[3]/normY;
      expanded[4] = expanded[4]/normY;
      expanded[5] = expanded[5]/normY;

      //Bind FrameBuffer
      glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
      /* Render here */
      // Clear the colorbuffer
      glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


      simpleShader.Use();

      //Transformations (Translate*Rotate*Scale)
      M = glm::mat4(1);//Init

      glm::vec3 myZ=cross(glm::vec3(expanded[0],expanded[1],expanded[2]),glm::vec3(expanded[3],expanded[4],expanded[5]) );
      // cout << "{" << myZ[0] << "," << myZ[1] << "," << myZ[2] << "} " << endl << endl;

      rotVecNew[0]=expanded[0];//x1
      rotVecNew[1]=expanded[3];//y1
      rotVecNew[2]=myZ[0];//z1

      rotVecNew[4]=expanded[1];//x2
      rotVecNew[5]=expanded[4];//y2
      rotVecNew[6]=myZ[1];//z2

      rotVecNew[8]=expanded[2];//x3
      rotVecNew[9]=expanded[5];//y3
      rotVecNew[10]=myZ[2];//z3

      memcpy( glm::value_ptr( rotMat ), rotVecNew, sizeof( rotVecNew ) );
      M = M * rotMat;
      M = glm::scale(M, glm::vec3(expanded[6],expanded[6],expanded[6]));//Scale

      simpleShader.BindMatrices(&M,&V,&P);

      ourModel.Draw(simpleShader);
      // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 80);
      frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
      distance=Loss(imageGray, frameImage, favg,cff);
      var_distance=last_distance-distance;
      last_distance=distance;
      distance_expan=distance;

      cout << "\ndownhill calcule expanded: distance:" <<distance << " var_distance: " << var_distance << endl;

      /* unbind frameBuffer */
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      if(distance_expan<distance_reflec)
      {
        insertionFancy(fw,distance_expan,expanded,w);
      }
      else
      {
        insertionFancy(fw,distance_reflec,reflect,w);
      }
    }
    //Contraction
    if(distance_reflec>fw[N_DIMENSIONS-1])//f(xr)>f(xn)
    {
      //Do Contraction

      contracted[0]=centroid[0]+gama*(w[(N_DIMENSIONS*N_DIMENSIONS)]-centroid[0]);
      contracted[1]=centroid[1]+gama*(w[(N_DIMENSIONS*N_DIMENSIONS)+1]-centroid[1]);
      contracted[2]=centroid[2]+gama*(w[(N_DIMENSIONS*N_DIMENSIONS)+2]-centroid[2]);

      contracted[3]=centroid[3]+gama*(w[(N_DIMENSIONS*N_DIMENSIONS)+3]-centroid[3]);
      contracted[4]=centroid[4]+gama*(w[(N_DIMENSIONS*N_DIMENSIONS)+4]-centroid[4]);
      contracted[5]=centroid[5]+gama*(w[(N_DIMENSIONS*N_DIMENSIONS)+5]-centroid[5]);

      contracted[6]=centroid[6]+gama*(w[(N_DIMENSIONS*N_DIMENSIONS)+6]-centroid[6]);

      normX=sqrt(contracted[0]*contracted[0]+
                      contracted[1]*contracted[1]+
                      contracted[2]*contracted[2]);

      normY=sqrt(contracted[3]*contracted[3]+
                      contracted[4]*contracted[4]+
                      contracted[5]*contracted[5]);
      contracted[0] = contracted[0]/normX;
      contracted[1] = contracted[1]/normX;
      contracted[2] = contracted[2]/normX;

      contracted[3] = contracted[3]/normY;
      contracted[4] = contracted[4]/normY;
      contracted[5] = contracted[5]/normY;

      //Bind FrameBuffer
      glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
      /* Render here */
      // Clear the colorbuffer
      glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


      simpleShader.Use();

      //Transformations (Translate*Rotate*Scale)
      M = glm::mat4(1);//Init

      glm::vec3 myZ=cross(glm::vec3(contracted[0],contracted[1],contracted[2]),glm::vec3(contracted[3],contracted[4],contracted[5]) );
      // cout << "{" << myZ[0] << "," << myZ[1] << "," << myZ[2] << "} " << endl << endl;

      rotVecNew[0]=contracted[0];//x1
      rotVecNew[1]=contracted[3];//y1
      rotVecNew[2]=myZ[0];//z1

      rotVecNew[4]=contracted[1];//x2
      rotVecNew[5]=contracted[4];//y2
      rotVecNew[6]=myZ[1];//z2

      rotVecNew[8]=contracted[2];//x3
      rotVecNew[9]=contracted[5];//y3
      rotVecNew[10]=myZ[2];//z3

      memcpy( glm::value_ptr( rotMat ), rotVecNew, sizeof( rotVecNew ) );
      M = M * rotMat;
      M = glm::scale(M, glm::vec3(contracted[6],contracted[6],contracted[6]) );//Scale

      simpleShader.BindMatrices(&M,&V,&P);

      ourModel.Draw(simpleShader);
      // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 80);
      frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
      distance=Loss(imageGray, frameImage, favg,cff);
      var_distance=last_distance-distance;
      last_distance=distance;
      distance_contrac=distance;

      cout << "\ndownhill calcule contract: distance:" <<distance << " var_distance: " << var_distance << endl;

      /* unbind frameBuffer */
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      if(distance_contrac<fw[N_DIMENSIONS])
      {
        insertionFancy(fw,distance_contrac,contracted,w);
      }
      else
      {
        //Shrink
        for(int j=1;j<N_DIMENSIONSSUM;j++)
        {
          shrink[0]=w[0]+sigma*(w[(j*N_DIMENSIONS)]-w[0]);
          shrink[1]=w[1]+sigma*(w[(j*N_DIMENSIONS)+1]-w[1]);
          shrink[2]=w[2]+sigma*(w[(j*N_DIMENSIONS)+2]-w[2]);

          shrink[3]=w[3]+sigma*(w[(j*N_DIMENSIONS)+3]-w[3]);
          shrink[4]=w[4]+sigma*(w[(j*N_DIMENSIONS)+4]-w[4]);
          shrink[5]=w[5]+sigma*(w[(j*N_DIMENSIONS)+5]-w[5]);

          shrink[6]=w[6]+sigma*(w[(j*N_DIMENSIONS)+6]-w[6]);

          normX=sqrt(shrink[0]*shrink[0]+
                      shrink[1]*shrink[1]+
                      shrink[2]*shrink[2]);

          normY=sqrt(shrink[3]*shrink[3]+
                      shrink[4]*shrink[4]+
                      shrink[5]*shrink[5]);

          shrink[0] = shrink[0]/normX;
          shrink[1] = shrink[1]/normX;
          shrink[2] = shrink[2]/normX;

          shrink[3] = shrink[3]/normY;
          shrink[4] = shrink[4]/normY;
          shrink[5] = shrink[5]/normY;

          //Bind FrameBuffer
          glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
          /* Render here */
          // Clear the colorbuffer
          glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


          simpleShader.Use();

          //Transformations (Translate*Rotate*Scale)
          M = glm::mat4(1);//Init

          glm::vec3 myZ=cross(glm::vec3(shrink[0],shrink[1],shrink[2]),glm::vec3(shrink[3],shrink[4],shrink[5]) );
          // cout << "{" << myZ[0] << "," << myZ[1] << "," << myZ[2] << "} " << endl << endl;

          rotVecNew[0]=shrink[0];//x1
          rotVecNew[1]=shrink[3];//y1
          rotVecNew[2]=myZ[0];//z1

          rotVecNew[4]=shrink[1];//x2
          rotVecNew[5]=shrink[4];//y2
          rotVecNew[6]=myZ[1];//z2

          rotVecNew[8]=shrink[2];//x3
          rotVecNew[9]=shrink[5];//y3
          rotVecNew[10]=myZ[2];//z3

          memcpy( glm::value_ptr( rotMat ), rotVecNew, sizeof( rotVecNew ) );
          M = M * rotMat;
          M = glm::scale(M, glm::vec3(shrink[6],shrink[6],shrink[6]) );//Scale

          simpleShader.BindMatrices(&M,&V,&P);

          ourModel.Draw(simpleShader);
          // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 80);
          frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
          distance=Loss(imageGray, frameImage, favg,cff);
          var_distance=last_distance-distance;
          last_distance=distance;
          distance_shrink=distance;

          cout << "\ndownhill calcule shrink: distance:" <<distance << " var_distance: " << var_distance << endl;

          /* unbind frameBuffer */
          glBindFramebuffer(GL_FRAMEBUFFER, 0);

          w[(j*N_DIMENSIONS)]=shrink[0];
          w[(j*N_DIMENSIONS)+1]=shrink[1];
          w[(j*N_DIMENSIONS)+2]=shrink[2];

          w[(j*N_DIMENSIONS)+3]=shrink[3];
          w[(j*N_DIMENSIONS)+4]=shrink[4];
          w[(j*N_DIMENSIONS)+5]=shrink[5];

          w[(j*N_DIMENSIONS)+6]=shrink[6];
          fw[j]=distance_shrink;
          // if(distance_shrink<fw[N_DIMENSIONS])
          // {
          //   insertionFancy(fw,distance_shrink,shrink,w);
          //   break;
          // }

        }//endfor
        BottomUpMergeSort(fw,N_DIMENSIONSSUM,w,N_DIMENSIONS);
      }//endelse
    }//endif

  }//endfor

  cout << " fw[0]="<<fw[0] << " fw[1]="<<fw[1] << " fw[2]="<<fw[2] << " fw[3]="<<fw[3] << " fw[4]="<<fw[4] << " fw[5]="<<fw[5] << " fw[6]="<<fw[6] << " fw[7]="<<fw[7] << endl;
  cout << "fw[0]="<<fw[0]<<" w[0]="<< w[0]<< " w[1]="<< w[1]<< " w[2]="<< w[2]<< " w[3]="<< w[3]<< " w[4]="<< w[4]<< " w[5]="<< w[5]<< " w[6]="<< w[6] << endl;

  //Bind FrameBuffer
  glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  /* Render here */
  // Clear the colorbuffer
  glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  simpleShader.Use();

  //Transformations (Translate*Rotate*Scale)
  M = glm::mat4(1);//Init

  glm::vec3 myZ=cross(glm::vec3(w[0],w[1],w[2]),glm::vec3(w[3],w[4],w[5]) );
  // cout << "{" << myZ[0] << "," << myZ[1] << "," << myZ[2] << "} " << endl << endl;

  rotVecNew[0]=w[0];//x1
  rotVecNew[1]=w[3];//y1
  rotVecNew[2]=myZ[0];//z1

  rotVecNew[4]=w[1];//x2
  rotVecNew[5]=w[4];//y2
  rotVecNew[6]=myZ[1];//z2

  rotVecNew[8]=w[2];//x3
  rotVecNew[9]=w[5];//y3
  rotVecNew[10]=myZ[2];//z3

  memcpy( glm::value_ptr( rotMat ), rotVecNew, sizeof( rotVecNew ) );
  M = M * rotMat;
  M = glm::scale(M, glm::vec3(w[6],w[6],w[6]) );//Scale

  simpleShader.BindMatrices(&M,&V,&P);

  ourModel.Draw(simpleShader);
  SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 666);
  frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  distance=Loss(imageGray, frameImage, favg,cff);

  cout << "\nGenerating Image" << endl;

  /* unbind frameBuffer */
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
