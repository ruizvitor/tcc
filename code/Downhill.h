#include <stdlib.h>
#define WINDOW_HEIGHT 1024
#define WINDOW_WIDTH 1024
#define MAX_ITER 200
#define MIN_ERROR 0.01

void insertionFancy(float* fx,float value, float *face, float* x)
{
  int j=-1;

  for(int i=0;i<7;i++)
  {
    // cout << "value:"<< value << " fx[" << i << "]:" << fx[i] << endl;
    if(value<fx[i])
    {
      for(int j=6;j>i;j--)
      {
        fx[j]=fx[j-1];

        x[(j*6)]=x[((j-1)*6)];
        x[(j*6)+1]=x[((j-1)*6)+1];
        x[(j*6)+2]=x[((j-1)*6)+2];

        x[(j*6)+3]=x[((j-1)*6)+3];
        x[(j*6)+4]=x[((j-1)*6)+4];
        x[(j*6)+5]=x[((j-1)*6)+5];
      }
      fx[i]=value;
      j=i;
      break;
    }
  }

  // cout << "\nfx[0]:" << fx[0]
  // << " fx[1]:" << fx[1]
  // << " fx[2]:" << fx[2]
  // << " fx[3]:" << fx[3]
  // << " fx[4]:" << fx[4]
  // << " fx[5]:" << fx[5]
  // << " fx[6]:" << fx[6] << endl;

  if(j!=-1)
  {
    x[(j*6)]=face[0];
    x[(j*6)+1]=face[1];
    x[(j*6)+2]=face[2];

    x[(j*6)+3]=face[3];
    x[(j*6)+4]=face[4];
    x[(j*6)+5]=face[5];
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
              glm::vec3 & myscale,
              double favg,
              double cff,
              Mat & imageGray
            )
{
  //Init Variables
  std::vector< unsigned char > frameImage;
  // float alfa=1.0;
  // float beta=0.5;
  float gama=0.5;
  float sigma=0.5;
  float distance=1.0;
  float distance_reflec=1.0;
  float distance_expan=1.0;
  float distance_contrac=1.0;
  float last_distance=1.0;
  float var_distance=1.0;
  float rotVecNew[16] = {
     1.0f, 0.0f, 0.0f, 0.0f,
     0.0f, 1.0f, 0.0f, 0.0f,
     0.0f, 0.0f, 1.0f, 0.0f,
     0.0f, 0.0f, 0.0f, 1.0f
  };
  glm::mat4 rotMat;
  float reflect[6];
  float expanded[6];
  float contracted[6];
  for(int i=0;(i<MAX_ITER)&&(distance>MIN_ERROR);i++)
  {
    //Calculate centroid
    float centroid[6];
    centroid[0]= (w[0]+w[6]+w[12]+w[18]+w[24]+w[30])/6;
    centroid[1]= (w[1]+w[7]+w[13]+w[19]+w[25]+w[31])/6;
    centroid[2]= (w[2]+w[8]+w[14]+w[20]+w[26]+w[32])/6;

    centroid[3]= (w[3]+w[9]+w[15]+w[21]+w[27]+w[33])/6;
    centroid[4]= (w[4]+w[10]+w[16]+w[22]+w[28]+w[34])/6;
    centroid[5]= (w[5]+w[11]+w[17]+w[23]+w[29]+w[35])/6;

    // cout << centroid[0] << " " << centroid[1] << " " << centroid[2] << endl;
    // cout << centroid[3] << " " << centroid[4] << " " << centroid[5] << endl << endl;
    //Reflection

    reflect[0] = centroid[0] + (centroid[0]-w[36]);
    reflect[1] = centroid[1] + (centroid[1]-w[37]);
    reflect[2] = centroid[2] + (centroid[2]-w[38]);

    reflect[3] = centroid[3] + (centroid[3]-w[39]);
    reflect[4] = centroid[4] + (centroid[4]-w[40]);
    reflect[5] = centroid[5] + (centroid[5]-w[41]);

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
    M = glm::scale(M, myscale);//Scale

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
    if((distance_reflec>fw[0])&&(distance_reflec<fw[5]))//f(x1)<f(xr)<f(xn)
    {
      insertionFancy(fw,distance_reflec,reflect,w);
    }

    //Expansion
    if(distance_reflec<fw[0])//f(xr)<f(x1)
    {
      //Do expansion
      //Calculate expanded

      expanded[0]=reflect[0]+(reflect[0]-centroid[0]);
      expanded[1]=reflect[1]+(reflect[1]-centroid[1]);
      expanded[2]=reflect[2]+(reflect[2]-centroid[2]);

      expanded[3]=reflect[3]+(reflect[3]-centroid[3]);
      expanded[4]=reflect[4]+(reflect[4]-centroid[4]);
      expanded[5]=reflect[5]+(reflect[5]-centroid[5]);

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
      M = glm::scale(M, myscale);//Scale

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
    if(distance_reflec>fw[5])//f(xr)>f(xn)
    {
      //Do Contraction

      contracted[0]=centroid[0]+gama*(w[36]-centroid[0]);
      contracted[1]=centroid[1]+gama*(w[37]-centroid[1]);
      contracted[2]=centroid[2]+gama*(w[38]-centroid[2]);

      contracted[3]=centroid[3]+gama*(w[39]-centroid[3]);
      contracted[4]=centroid[4]+gama*(w[40]-centroid[4]);
      contracted[5]=centroid[5]+gama*(w[41]-centroid[5]);

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
      M = glm::scale(M, myscale);//Scale

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
      if(distance_contrac<fw[6])
      {
        insertionFancy(fw,distance_contrac,contracted,w);
      }
      else
      {
        //Shrink
        for(int j=1;j<7;j++)
        {
          w[(j*6)]=w[0]+sigma*(w[(j*6)]-w[0]);
          w[(j*6)+1]=w[1]+sigma*(w[(j*6)+1]-w[1]);
          w[(j*6)+2]=w[2]+sigma*(w[(j*6)+2]-w[2]);

          w[(j*6)+3]=w[3]+sigma*(w[(j*6)+3]-w[3]);
          w[(j*6)+4]=w[4]+sigma*(w[(j*6)+4]-w[4]);
          w[(j*6)+5]=w[5]+sigma*(w[(j*6)+5]-w[5]);

          normX=sqrt(w[(j*6)]*w[(j*6)]+
                      w[(j*6)+1]*w[(j*6)+1]+
                      w[(j*6)+2]*w[(j*6)+2]);

          normY=sqrt(w[(j*6)+3]*w[(j*6)+3]+
                      w[(j*6)+4]*w[(j*6)+4]+
                      w[(j*6)+5]*w[(j*6)+5]);

          w[(j*6)] = w[(j*6)+0]/normX;
          w[(j*6)+1] = w[(j*6)+1]/normX;
          w[(j*6)+2] = w[(j*6)+2]/normX;

          w[(j*6)+3] = w[(j*6)+3]/normY;
          w[(j*6)+4] = w[(j*6)+4]/normY;
          w[(j*6)+5] = w[(j*6)+5]/normY;

          //Bind FrameBuffer
          glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
          /* Render here */
          // Clear the colorbuffer
          glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


          simpleShader.Use();

          //Transformations (Translate*Rotate*Scale)
          M = glm::mat4(1);//Init

          glm::vec3 myZ=cross(glm::vec3(w[(j*6)],w[(j*6)+1],w[(j*6)+2]),glm::vec3(w[(j*6)+3],w[(j*6)+4],w[(j*6)+5]) );
          // cout << "{" << myZ[0] << "," << myZ[1] << "," << myZ[2] << "} " << endl << endl;

          rotVecNew[0]=w[(j*6)];//x1
          rotVecNew[1]=w[(j*6)+3];//y1
          rotVecNew[2]=myZ[0];//z1

          rotVecNew[4]=w[(j*6)+1];//x2
          rotVecNew[5]=w[(j*6)+4];//y2
          rotVecNew[6]=myZ[1];//z2

          rotVecNew[8]=w[(j*6)+2];//x3
          rotVecNew[9]=w[(j*6)+5];//y3
          rotVecNew[10]=myZ[2];//z3

          memcpy( glm::value_ptr( rotMat ), rotVecNew, sizeof( rotVecNew ) );
          M = M * rotMat;
          M = glm::scale(M, myscale);//Scale

          simpleShader.BindMatrices(&M,&V,&P);

          ourModel.Draw(simpleShader);
          // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 80);
          frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
          distance=Loss(imageGray, frameImage, favg,cff);
          var_distance=last_distance-distance;
          last_distance=distance;
          fw[j]=distance;

          cout << "\ndownhill calcule shrink: distance:" <<distance << " var_distance: " << var_distance << endl;

          /* unbind frameBuffer */
          glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }//endfor
      }//endelse
    }//endif

  }//endfor

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
  M = glm::scale(M, myscale);//Scale

  simpleShader.BindMatrices(&M,&V,&P);

  ourModel.Draw(simpleShader);
  SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 666);
  frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  distance=Loss(imageGray, frameImage, favg,cff);

  cout << "\nGenerating Image" << endl;

  /* unbind frameBuffer */
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
