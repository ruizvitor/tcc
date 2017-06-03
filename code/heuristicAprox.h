#include "Macros.h"

void cubeAprox(float* fw,
              float* w,
              GLuint myFrameBuffer,
              Shader & simpleShader,
              Model & ourModel,
              glm::mat4 & M,
              glm::mat4 & V,
              glm::mat4 & P,
              double favg,
              double cff,
              Mat & imageGray,
              float firstscale,
              float firstx,
              float firsty,
              glm::mat4 initialRot
            )
{
  cout<<"--cubeAprox--"<<endl;

  glm::mat4 rotMat;
  float distance=1.0;
  float tempVec[N_DIMENSIONS];
  std::vector< unsigned char > frameImage;

  //Bind FrameBuffer
  glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  /* Render here */
  // Clear the colorbuffer
  glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  simpleShader.Use();

  //Transformations (Translate*Rotate*Scale)
  M = glm::mat4(1);//Init
  M = glm::translate(M, glm::vec3(firstx,firsty,0.0f));//Translate
  rotMat=initialRot;
  // rotMat = glm::rotate(rotMat, glm::radians(-15.0f*k), glm::vec3(0.0f,0.0f,1.0f));//Rotate
  // rotMat = glm::rotate(rotMat, glm::radians(-15.0f*i), glm::vec3(0.0f,1.0f,0.0f));//Rotate
  // rotMat = glm::rotate(rotMat, glm::radians(-15.0f*j), glm::vec3(1.0f,0.0f,0.0f));//Rotate
  M = M * rotMat;
  M = glm::scale(M, glm::vec3(firstscale,firstscale,firstscale));//Scale

  simpleShader.BindMatrices(&M,&V,&P);
  ourModel.Draw(simpleShader);
  SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 777 );

  /* unbind frameBuffer */
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  for(int i=0;i<3;i++)
  {
    for(int j=0;j<3;j++)
    {
      for(int k=0;k<3;k++)
      {
        //Bind FrameBuffer
        glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
        /* Render here */
        // Clear the colorbuffer
        glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        simpleShader.Use();

        //Transformations (Translate*Rotate*Scale)
        M = glm::mat4(1);//Init
        M = glm::translate(M, glm::vec3(firstx,firsty,0.0f));//Translate
        rotMat = initialRot;
        rotMat = glm::rotate(rotMat, glm::radians(-15.0f*k), glm::vec3(0.0f,0.0f,1.0f));//Rotate
        rotMat = glm::rotate(rotMat, glm::radians(-15.0f*i), glm::vec3(0.0f,1.0f,0.0f));//Rotate
        rotMat = glm::rotate(rotMat, glm::radians(-15.0f*j), glm::vec3(1.0f,0.0f,0.0f));//Rotate
        M = M * rotMat;
        M = glm::scale(M, glm::vec3(firstscale,firstscale,firstscale));//Scale

        simpleShader.BindMatrices(&M,&V,&P);
        // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, (100*i)+(10*j)+k );
        ourModel.Draw(simpleShader);
        frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
        distance=Loss(imageGray, frameImage, favg,cff);

        cout << distance << endl;

        tempVec[0]=rotMat[0][0];
        tempVec[1]=rotMat[1][0];
        tempVec[2]=rotMat[2][0];

        tempVec[3]=rotMat[0][1];
        tempVec[4]=rotMat[1][1];
        tempVec[5]=rotMat[2][1];

        tempVec[6]=firstscale;
        tempVec[7]=firstx;
        tempVec[8]=firsty;
        insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);

        /* unbind frameBuffer */
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
      }
    }
  }

  for(int i=0;i<3;i++)
  {
    for(int j=0;j<3;j++)
    {
      for(int k=0;k<3;k++)
      {
        //Bind FrameBuffer
        glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
        /* Render here */
        // Clear the colorbuffer
        glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        simpleShader.Use();

        //Transformations (Translate*Rotate*Scale)
        M = glm::mat4(1);//Init
        M = glm::translate(M, glm::vec3(firstx,firsty,0.0f));//Translate
        rotMat = initialRot;
        rotMat = glm::rotate(rotMat, glm::radians(15.0f*k), glm::vec3(0.0f,0.0f,1.0f));//Rotate
        rotMat = glm::rotate(rotMat, glm::radians(15.0f*i), glm::vec3(0.0f,1.0f,0.0f));//Rotate
        rotMat = glm::rotate(rotMat, glm::radians(15.0f*j), glm::vec3(1.0f,0.0f,0.0f));//Rotate
        M = M * rotMat;
        M = glm::scale(M, glm::vec3(firstscale,firstscale,firstscale));//Scale

        simpleShader.BindMatrices(&M,&V,&P);

        ourModel.Draw(simpleShader);
        // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 1000+(100*i)+(10*j)+k );
        frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
        distance=Loss(imageGray, frameImage, favg,cff);

        cout << distance << endl;

        tempVec[0]=rotMat[0][0];
        tempVec[1]=rotMat[1][0];
        tempVec[2]=rotMat[2][0];

        tempVec[3]=rotMat[0][1];
        tempVec[4]=rotMat[1][1];
        tempVec[5]=rotMat[2][1];

        tempVec[6]=firstscale;
        tempVec[7]=firstx;
        tempVec[8]=firsty;
        insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);

        /* unbind frameBuffer */
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
      }
    }
  }
}

void scaleAprox(float* fw,
              float* w,
              GLuint myFrameBuffer,
              Shader & simpleShader,
              Model & ourModel,
              glm::mat4 & M,
              glm::mat4 & V,
              glm::mat4 & P,
              double favg,
              double cff,
              Mat & imageGray,
              float firstscale,
              float firstx,
              float firsty
            )
{
  cout<<"--scaleAprox--"<<endl;
  std::vector< unsigned char > frameImage;
  glm::mat4 rotMat;
  float distance=1.0;
  float tempVec[N_DIMENSIONS];

  for(int k=1;k<5;k++)
  {
    //Bind FrameBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
    /* Render here */
    // Clear the colorbuffer
    glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    simpleShader.Use();

    //Transformations (Translate*Rotate*Scale)
    M = glm::mat4(1);//Init
    M = glm::translate(M, glm::vec3(firstx,firsty,0.0f));//Translate

    glm::vec3 myZ=cross(glm::vec3(w[0],w[1],w[2]),glm::vec3(w[3],w[4],w[5]) );

    rotMat[0][0]=w[0];//x1
    rotMat[1][0]=w[3];//y1
    rotMat[2][0]=myZ[0];//z1

    rotMat[0][1]=w[1];//x2
    rotMat[1][1]=w[4];//y2
    rotMat[2][1]=myZ[1];//z2

    rotMat[0][2]=w[2];//x3
    rotMat[1][2]=w[5];//y3
    rotMat[2][2]=myZ[2];//z3

    M = M * rotMat;
    M = glm::scale(M, glm::vec3(firstscale-3*k,firstscale-3*k,firstscale-3*k));//Scale

    simpleShader.BindMatrices(&M,&V,&P);

    ourModel.Draw(simpleShader);
    // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, k);
    frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
    distance=Loss(imageGray, frameImage, favg,cff);

    cout << distance << endl;

    tempVec[0]=rotMat[0][0];
    tempVec[1]=rotMat[1][0];
    tempVec[2]=rotMat[2][0];

    tempVec[3]=rotMat[0][1];
    tempVec[4]=rotMat[1][1];
    tempVec[5]=rotMat[2][1];

    tempVec[6]=firstscale-3*k;
    tempVec[7]=firstx;
    tempVec[8]=firsty;
    insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);

    /* unbind frameBuffer */
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  for(int k=1;k<5;k++)
  {
    //Bind FrameBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
    /* Render here */
    // Clear the colorbuffer
    glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    simpleShader.Use();

    //Transformations (Translate*Rotate*Scale)
    M = glm::mat4(1);//Init
    M = glm::translate(M, glm::vec3(firstx,firsty,0.0f));//Translate

    glm::vec3 myZ=cross(glm::vec3(w[0],w[1],w[2]),glm::vec3(w[3],w[4],w[5]) );

    rotMat[0][0]=w[0];//x1
    rotMat[1][0]=w[3];//y1
    rotMat[2][0]=myZ[0];//z1

    rotMat[0][1]=w[1];//x2
    rotMat[1][1]=w[4];//y2
    rotMat[2][1]=myZ[1];//z2

    rotMat[0][2]=w[2];//x3
    rotMat[1][2]=w[5];//y3
    rotMat[2][2]=myZ[2];//z3
    M = M * rotMat;
    M = glm::scale(M, glm::vec3(firstscale+3*k,firstscale+3*k,firstscale+3*k));//Scale

    simpleShader.BindMatrices(&M,&V,&P);

    ourModel.Draw(simpleShader);
    // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 10+k);
    frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
    distance=Loss(imageGray, frameImage, favg,cff);

    cout << distance << endl;

    tempVec[0]=rotMat[0][0];
    tempVec[1]=rotMat[1][0];
    tempVec[2]=rotMat[2][0];

    tempVec[3]=rotMat[0][1];
    tempVec[4]=rotMat[1][1];
    tempVec[5]=rotMat[2][1];

    tempVec[6]=firstscale+3*k;
    tempVec[7]=firstx;
    tempVec[8]=firsty;
    insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);

    /* unbind frameBuffer */
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  cout<<"--xyAprox--"<<endl;
  //Bind FrameBuffer
  glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  /* Render here */
  // Clear the colorbuffer
  glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  simpleShader.Use();

  //Transformations (Translate*Rotate*Scale)
  M = glm::mat4(1);//Init
  M = glm::translate(M, glm::vec3(firstx-MOVEFACTOR,0.0f,0.0f));//Translate

  glm::vec3 myZ=cross(glm::vec3(w[0],w[1],w[2]),glm::vec3(w[3],w[4],w[5]) );

  rotMat[0][0]=w[0];//x1
  rotMat[1][0]=w[3];//y1
  rotMat[2][0]=myZ[0];//z1

  rotMat[0][1]=w[1];//x2
  rotMat[1][1]=w[4];//y2
  rotMat[2][1]=myZ[1];//z2

  rotMat[0][2]=w[2];//x3
  rotMat[1][2]=w[5];//y3
  rotMat[2][2]=myZ[2];//z3
  M = M * rotMat;
  M = glm::scale(M, glm::vec3(w[6],w[6],w[6]));//Scale

  simpleShader.BindMatrices(&M,&V,&P);

  ourModel.Draw(simpleShader);
  // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 10+k);
  frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  distance=Loss(imageGray, frameImage, favg,cff);

  cout << distance << endl;

  tempVec[0]=rotMat[0][0];
  tempVec[1]=rotMat[1][0];
  tempVec[2]=rotMat[2][0];

  tempVec[3]=rotMat[0][1];
  tempVec[4]=rotMat[1][1];
  tempVec[5]=rotMat[2][1];

  tempVec[6]=w[6];
  tempVec[7]=firstx-MOVEFACTOR;
  tempVec[8]=0;
  insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);

  /* unbind frameBuffer */
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //Bind FrameBuffer
  glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  /* Render here */
  // Clear the colorbuffer
  glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  simpleShader.Use();

  //Transformations (Translate*Rotate*Scale)
  M = glm::mat4(1);//Init
  M = glm::translate(M, glm::vec3(firstx+MOVEFACTOR,0.0f,0.0f));//Translate

  myZ=cross(glm::vec3(w[0],w[1],w[2]),glm::vec3(w[3],w[4],w[5]) );

  rotMat[0][0]=w[0];//x1
  rotMat[1][0]=w[3];//y1
  rotMat[2][0]=myZ[0];//z1

  rotMat[0][1]=w[1];//x2
  rotMat[1][1]=w[4];//y2
  rotMat[2][1]=myZ[1];//z2

  rotMat[0][2]=w[2];//x3
  rotMat[1][2]=w[5];//y3
  rotMat[2][2]=myZ[2];//z3
  M = M * rotMat;
  M = glm::scale(M, glm::vec3(w[6],w[6],w[6]));//Scale

  simpleShader.BindMatrices(&M,&V,&P);

  ourModel.Draw(simpleShader);
  // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 10+k);
  frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  distance=Loss(imageGray, frameImage, favg,cff);

  cout << distance << endl;

  tempVec[0]=rotMat[0][0];
  tempVec[1]=rotMat[1][0];
  tempVec[2]=rotMat[2][0];

  tempVec[3]=rotMat[0][1];
  tempVec[4]=rotMat[1][1];
  tempVec[5]=rotMat[2][1];
  tempVec[6]=w[6];
  tempVec[7]=firstx+MOVEFACTOR;
  tempVec[8]=0;
  insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);

  /* unbind frameBuffer */
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //Bind FrameBuffer
  glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  /* Render here */
  // Clear the colorbuffer
  glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  simpleShader.Use();

  //Transformations (Translate*Rotate*Scale)
  M = glm::mat4(1);//Init
  M = glm::translate(M, glm::vec3(0.0f,firsty-MOVEFACTOR,0.0f));//Translate

  myZ=cross(glm::vec3(w[0],w[1],w[2]),glm::vec3(w[3],w[4],w[5]) );

  rotMat[0][0]=w[0];//x1
  rotMat[1][0]=w[3];//y1
  rotMat[2][0]=myZ[0];//z1

  rotMat[0][1]=w[1];//x2
  rotMat[1][1]=w[4];//y2
  rotMat[2][1]=myZ[1];//z2

  rotMat[0][2]=w[2];//x3
  rotMat[1][2]=w[5];//y3
  rotMat[2][2]=myZ[2];//z3
  M = M * rotMat;
  M = glm::scale(M, glm::vec3(w[6],w[6],w[6]));//Scale

  simpleShader.BindMatrices(&M,&V,&P);

  ourModel.Draw(simpleShader);
  // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 10+k);
  frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  distance=Loss(imageGray, frameImage, favg,cff);

  cout << distance << endl;

  tempVec[0]=rotMat[0][0];
  tempVec[1]=rotMat[1][0];
  tempVec[2]=rotMat[2][0];

  tempVec[3]=rotMat[0][1];
  tempVec[4]=rotMat[1][1];
  tempVec[5]=rotMat[2][1];
  tempVec[6]=w[6];
  tempVec[7]=0.0f;
  tempVec[8]=firsty-MOVEFACTOR;
  insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);

  /* unbind frameBuffer */
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //Bind FrameBuffer
  glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  /* Render here */
  // Clear the colorbuffer
  glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  simpleShader.Use();

  //Transformations (Translate*Rotate*Scale)
  M = glm::mat4(1);//Init
  M = glm::translate(M, glm::vec3(0.0f,firsty+MOVEFACTOR,0.0f));//Translate

  myZ=cross(glm::vec3(w[0],w[1],w[2]),glm::vec3(w[3],w[4],w[5]) );

  rotMat[0][0]=w[0];//x1
  rotMat[1][0]=w[3];//y1
  rotMat[2][0]=myZ[0];//z1

  rotMat[0][1]=w[1];//x2
  rotMat[1][1]=w[4];//y2
  rotMat[2][1]=myZ[1];//z2

  rotMat[0][2]=w[2];//x3
  rotMat[1][2]=w[5];//y3
  rotMat[2][2]=myZ[2];//z3
  M = M * rotMat;
  M = glm::scale(M, glm::vec3(w[6],w[6],w[6]));//Scale

  simpleShader.BindMatrices(&M,&V,&P);

  ourModel.Draw(simpleShader);
  // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 10+k);
  frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  distance=Loss(imageGray, frameImage, favg,cff);

  cout << distance << endl;

  tempVec[0]=rotMat[0][0];
  tempVec[1]=rotMat[1][0];
  tempVec[2]=rotMat[2][0];

  tempVec[3]=rotMat[0][1];
  tempVec[4]=rotMat[1][1];
  tempVec[5]=rotMat[2][1];
  tempVec[6]=w[6];
  tempVec[7]=0.0f;
  tempVec[8]=firsty+MOVEFACTOR;
  insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);

  /* unbind frameBuffer */
  glBindFramebuffer(GL_FRAMEBUFFER, 0);


  //Bind FrameBuffer
  glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  /* Render here */
  // Clear the colorbuffer
  glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  simpleShader.Use();

  //Transformations (Translate*Rotate*Scale)
  M = glm::mat4(1);//Init
  M = glm::translate(M, glm::vec3(firstx+MOVEFACTOR,firsty+MOVEFACTOR,0.0f));//Translate

  myZ=cross(glm::vec3(w[0],w[1],w[2]),glm::vec3(w[3],w[4],w[5]) );

  rotMat[0][0]=w[0];//x1
  rotMat[1][0]=w[3];//y1
  rotMat[2][0]=myZ[0];//z1

  rotMat[0][1]=w[1];//x2
  rotMat[1][1]=w[4];//y2
  rotMat[2][1]=myZ[1];//z2

  rotMat[0][2]=w[2];//x3
  rotMat[1][2]=w[5];//y3
  rotMat[2][2]=myZ[2];//z3
  M = M * rotMat;
  M = glm::scale(M, glm::vec3(w[6],w[6],w[6]));//Scale

  simpleShader.BindMatrices(&M,&V,&P);

  ourModel.Draw(simpleShader);
  // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 10+k);
  frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  distance=Loss(imageGray, frameImage, favg,cff);

  cout << distance << endl;

  tempVec[0]=rotMat[0][0];
  tempVec[1]=rotMat[1][0];
  tempVec[2]=rotMat[2][0];

  tempVec[3]=rotMat[0][1];
  tempVec[4]=rotMat[1][1];
  tempVec[5]=rotMat[2][1];
  tempVec[6]=w[6];
  tempVec[7]=firstx+MOVEFACTOR;
  tempVec[8]=firsty+MOVEFACTOR;
  insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);

  /* unbind frameBuffer */
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //Bind FrameBuffer
  glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  /* Render here */
  // Clear the colorbuffer
  glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  simpleShader.Use();

  //Transformations (Translate*Rotate*Scale)
  M = glm::mat4(1);//Init
  M = glm::translate(M, glm::vec3(firstx-MOVEFACTOR,firsty-MOVEFACTOR,0.0f));//Translate

  myZ=cross(glm::vec3(w[0],w[1],w[2]),glm::vec3(w[3],w[4],w[5]) );

  rotMat[0][0]=w[0];//x1
  rotMat[1][0]=w[3];//y1
  rotMat[2][0]=myZ[0];//z1

  rotMat[0][1]=w[1];//x2
  rotMat[1][1]=w[4];//y2
  rotMat[2][1]=myZ[1];//z2

  rotMat[0][2]=w[2];//x3
  rotMat[1][2]=w[5];//y3
  rotMat[2][2]=myZ[2];//z3
  M = M * rotMat;
  M = glm::scale(M, glm::vec3(w[6],w[6],w[6]));//Scale

  simpleShader.BindMatrices(&M,&V,&P);

  ourModel.Draw(simpleShader);
  // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 10+k);
  frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  distance=Loss(imageGray, frameImage, favg,cff);

  cout << distance << endl;

  tempVec[0]=rotMat[0][0];
  tempVec[1]=rotMat[1][0];
  tempVec[2]=rotMat[2][0];

  tempVec[3]=rotMat[0][1];
  tempVec[4]=rotMat[1][1];
  tempVec[5]=rotMat[2][1];
  tempVec[6]=w[6];
  tempVec[7]=firstx-MOVEFACTOR;
  tempVec[8]=firsty-MOVEFACTOR;
  insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);

  /* unbind frameBuffer */
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //Bind FrameBuffer
  glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  /* Render here */
  // Clear the colorbuffer
  glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  simpleShader.Use();

  //Transformations (Translate*Rotate*Scale)
  M = glm::mat4(1);//Init
  M = glm::translate(M, glm::vec3(firstx-MOVEFACTOR,firsty+MOVEFACTOR,0.0f));//Translate

  myZ=cross(glm::vec3(w[0],w[1],w[2]),glm::vec3(w[3],w[4],w[5]) );

  rotMat[0][0]=w[0];//x1
  rotMat[1][0]=w[3];//y1
  rotMat[2][0]=myZ[0];//z1

  rotMat[0][1]=w[1];//x2
  rotMat[1][1]=w[4];//y2
  rotMat[2][1]=myZ[1];//z2

  rotMat[0][2]=w[2];//x3
  rotMat[1][2]=w[5];//y3
  rotMat[2][2]=myZ[2];//z3
  M = M * rotMat;
  M = glm::scale(M, glm::vec3(w[6],w[6],w[6]));//Scale

  simpleShader.BindMatrices(&M,&V,&P);

  ourModel.Draw(simpleShader);
  // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 10+k);
  frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  distance=Loss(imageGray, frameImage, favg,cff);

  cout << distance << endl;

  tempVec[0]=rotMat[0][0];
  tempVec[1]=rotMat[1][0];
  tempVec[2]=rotMat[2][0];

  tempVec[3]=rotMat[0][1];
  tempVec[4]=rotMat[1][1];
  tempVec[5]=rotMat[2][1];
  tempVec[6]=w[6];
  tempVec[7]=firstx-MOVEFACTOR;
  tempVec[8]=firsty+MOVEFACTOR;
  insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);

  /* unbind frameBuffer */
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //Bind FrameBuffer
  glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  /* Render here */
  // Clear the colorbuffer
  glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  simpleShader.Use();

  //Transformations (Translate*Rotate*Scale)
  M = glm::mat4(1);//Init
  M = glm::translate(M, glm::vec3(firstx+MOVEFACTOR,firsty-MOVEFACTOR,0.0f));//Translate

  myZ=cross(glm::vec3(w[0],w[1],w[2]),glm::vec3(w[3],w[4],w[5]) );

  rotMat[0][0]=w[0];//x1
  rotMat[1][0]=w[3];//y1
  rotMat[2][0]=myZ[0];//z1

  rotMat[0][1]=w[1];//x2
  rotMat[1][1]=w[4];//y2
  rotMat[2][1]=myZ[1];//z2

  rotMat[0][2]=w[2];//x3
  rotMat[1][2]=w[5];//y3
  rotMat[2][2]=myZ[2];//z3
  M = M * rotMat;
  M = glm::scale(M, glm::vec3(w[6],w[6],w[6]));//Scale

  simpleShader.BindMatrices(&M,&V,&P);

  ourModel.Draw(simpleShader);
  // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 10+k);
  frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  distance=Loss(imageGray, frameImage, favg,cff);

  cout << distance << endl;

  tempVec[0]=rotMat[0][0];
  tempVec[1]=rotMat[1][0];
  tempVec[2]=rotMat[2][0];

  tempVec[3]=rotMat[0][1];
  tempVec[4]=rotMat[1][1];
  tempVec[5]=rotMat[2][1];
  tempVec[6]=w[6];
  tempVec[7]=firstx+MOVEFACTOR;
  tempVec[8]=firsty-MOVEFACTOR;
  insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);

  /* unbind frameBuffer */
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void cubefaces(float* fw,
              float* w,
              GLuint myFrameBuffer,
              Shader & simpleShader,
              Model & ourModel,
              glm::mat4 & M,
              glm::mat4 & V,
              glm::mat4 & P,
              double favg,
              double cff,
              Mat & imageGray,
              float firstscale,
              glm::mat4 &initialRot,
              float firstx,
              float firsty
            )
{
  std::vector< unsigned char > frameImage;
  glm::mat4 rotMat;
  float distance=1.0;
  float tempVec[N_DIMENSIONS];

  cout<<"--Cubeface--"<<endl;
  //CUBEFACES
  for(int j=0;j<4;j++)
  {
    // for(int k=0;k<4;k++)
    // {
      //Bind FrameBuffer
      glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
      /* Render here */
      // Clear the colorbuffer
      glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


      simpleShader.Use();

      //Transformations (Translate*Rotate*Scale)
      M = glm::mat4(1);//Init
      M = glm::translate(M, glm::vec3(firstx,firsty,0.0f));//Translate
      rotMat = initialRot;
      // rotMat = glm::rotate(rotMat, glm::radians(90.0f*k), glm::vec3(0.0f,0.0f,1.0f));//Rotate
      rotMat = glm::rotate(rotMat, glm::radians(90.0f*j), glm::vec3(1.0f,0.0f,0.0f));//Rotate
      M = M * rotMat;
      M = glm::scale(M, glm::vec3(firstscale,firstscale,firstscale));//Scale

      simpleShader.BindMatrices(&M,&V,&P);

      ourModel.Draw(simpleShader);
      // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, (j));
      frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
      distance=Loss(imageGray, frameImage, favg,cff);


      cout << distance << endl;

      tempVec[0]=rotMat[0][0];
      tempVec[1]=rotMat[1][0];
      tempVec[2]=rotMat[2][0];

      tempVec[3]=rotMat[0][1];
      tempVec[4]=rotMat[1][1];
      tempVec[5]=rotMat[2][1];

      tempVec[6]=firstscale;
      tempVec[7]=firstx;
      tempVec[8]=firsty;
      insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);

      /* unbind frameBuffer */
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // }
  }



  for(int j=1;j<4;j+=2)
  {
    // for(int k=0;k<4;k++)
    // {
      //Bind FrameBuffer
      glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
      /* Render here */
      // Clear the colorbuffer
      glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


      simpleShader.Use();

      //Transformations (Translate*Rotate*Scale)
      M = glm::mat4(1);//Init
      M = glm::translate(M, glm::vec3(firstx,firsty,0.0f));//Translate
      rotMat = initialRot;
      // rotMat = glm::rotate(rotMat, glm::radians(90.0f*k), glm::vec3(0.0f,0.0f,1.0f));//Rotate
      rotMat = glm::rotate(rotMat, glm::radians(90.0f*j), glm::vec3(0.0f,1.0f,0.0f));//Rotate
      M = M * rotMat;
      M = glm::scale(M, glm::vec3(firstscale,firstscale,firstscale));//Scale

      simpleShader.BindMatrices(&M,&V,&P);

      ourModel.Draw(simpleShader);
      // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 10+j);
      frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
      distance=Loss(imageGray, frameImage, favg,cff);

      cout << distance << endl;

      tempVec[0]=rotMat[0][0];
      tempVec[1]=rotMat[1][0];
      tempVec[2]=rotMat[2][0];

      tempVec[3]=rotMat[0][1];
      tempVec[4]=rotMat[1][1];
      tempVec[5]=rotMat[2][1];

      tempVec[6]=firstscale;
      tempVec[7]=firstx;
      tempVec[8]=firsty;
      insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);

      /* unbind frameBuffer */
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // }
  }
  glm::vec3 myZ=cross(glm::vec3(w[0],w[1],w[2]),glm::vec3(w[3],w[4],w[5]) );
  // cout << "\nGenerating Image" << endl;
  cout << "\nfw[0]="<<fw[0];
  cout << "float rotVecNew[16] = {" << endl
  << w[0] << ", "<< w[3] << ", " << myZ[0]<< ", 0.0f,\n"
  << w[1] << ", "<< w[4] << ", " << myZ[1]<< ", 0.0f,\n"
  << w[2] << ", "<< w[5] << ", " << myZ[2]<< ", 0.0f,\n"
  << "0.0f, 0.0f, 0.0f, 1.0f\n};\n"

  << "firstscale="   << w[6]<< ";\n"
  << "float firstx=" << w[7]<< ";\n"
  << "float firsty=" << w[8]<< ";\n\n";

  glm::vec3 myZz=cross(glm::vec3(w[9],w[10],w[11]),glm::vec3(w[12],w[13],w[14]) );
  cout << "\nfw[0]="<<fw[1];
  cout << "float rotVecNew[16] = {" << endl
  << w[9] << ", "<< w[12] << ", " << myZz[0]<< ", 0.0f,\n"
  << w[10] << ", "<< w[13] << ", " << myZz[1]<< ", 0.0f,\n"
  << w[11] << ", "<< w[14] << ", " << myZz[2]<< ", 0.0f,\n"
  << "0.0f, 0.0f, 0.0f, 1.0f\n};\n"

  << "firstscale="   << w[15]<< ";\n"
  << "float firstx=" << w[16]<< ";\n"
  << "float firsty=" << w[17]<< ";\n\n";

  float centroid[N_DIMENSIONS];
  //average of the 2 best faces
  centroid[0]= (w[0]+w[N_DIMENSIONS])/2;
  centroid[1]= (w[1]+w[N_DIMENSIONS+1])/2;
  centroid[2]= (w[2]+w[N_DIMENSIONS+2])/2;

  centroid[3]= (w[3]+w[N_DIMENSIONS+3])/2;
  centroid[4]= (w[4]+w[N_DIMENSIONS+4])/2;
  centroid[5]= (w[5]+w[N_DIMENSIONS+5])/2;

  centroid[6]= (w[6]+w[N_DIMENSIONS+6])/2;

  centroid[7]= (w[7]+w[N_DIMENSIONS+7])/2;
  centroid[8]= (w[8]+w[N_DIMENSIONS+8])/2;

  float normX=sqrt(centroid[0]*centroid[0]+
                  centroid[1]*centroid[1]+
                  centroid[2]*centroid[2]);

  float normY=sqrt(centroid[3]*centroid[3]+
                  centroid[4]*centroid[4]+
                  centroid[5]*centroid[5]);
  centroid[0] = centroid[0]/normX;
  centroid[1] = centroid[1]/normX;
  centroid[2] = centroid[2]/normX;

  centroid[3] = centroid[3]/normY;
  centroid[4] = centroid[4]/normY;
  centroid[5] = centroid[5]/normY;
  myZ=cross(glm::vec3(centroid[0],centroid[1],centroid[2]),glm::vec3(centroid[3],centroid[4],centroid[5]) );
  //Bind FrameBuffer
  glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  /* Render here */
  // Clear the colorbuffer
  glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  simpleShader.Use();

  //Transformations (Translate*Rotate*Scale)
  M = glm::mat4(1);//Init
  M = glm::translate(M, glm::vec3(centroid[7],centroid[8],0.0f));//Translate


  // rotVecNew[0]=w[0];//x1
  // rotVecNew[1]=w[3];//y1
  // rotVecNew[2]=myZ[0];//z1
  //
  // rotVecNew[4]=w[1];//x2
  // rotVecNew[5]=w[4];//y2
  // rotVecNew[6]=myZ[1];//z2
  //
  // rotVecNew[8]=w[2];//x3
  // rotVecNew[9]=w[5];//y3
  // rotVecNew[10]=myZ[2];//z3

  initialRot[0][0]=centroid[0];//x1
  initialRot[1][0]=centroid[3];//y1
  initialRot[2][0]=myZ[0];//z1

  initialRot[0][1]=centroid[1];//x2
  initialRot[1][1]=centroid[4];//y2
  initialRot[2][1]=myZ[1];//z2

  initialRot[0][2]=centroid[2];//x3
  initialRot[1][2]=centroid[5];//y3
  initialRot[2][2]=myZ[2];//z3

  w[0]=centroid[0];
  w[1]=centroid[1];
  w[2]=centroid[2];

  w[3]=centroid[3];
  w[4]=centroid[4];
  w[5]=centroid[5];

  w[6]=centroid[6];

  w[7]=centroid[7];
  w[8]=centroid[8];

  // memcpy( glm::value_ptr( rotMat ), rotVecNew, sizeof( rotVecNew ) );
  M = M * initialRot;
  M = glm::scale(M, glm::vec3(centroid[6],centroid[6],centroid[6]));//Scale

  simpleShader.BindMatrices(&M,&V,&P);

  ourModel.Draw(simpleShader);
  SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 888);
  frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  distance=Loss(imageGray, frameImage, favg,cff);

  cout << distance << endl;


  cout << "\nGenerating Image" << endl;
  cout
  << "initialRot[0][0]=" << w[0] << ";\n"
  << "initialRot[0][1]=" << w[3] << ";\n"
  << "initialRot[0][2]=" << myZ[0] << ";\n"

  << "initialRot[1][0]=" << w[1] << ";\n"
  << "initialRot[1][1]=" << w[4] << ";\n"
  << "initialRot[1][2]=" << myZ[1] << ";\n"

  << "initialRot[2][0]=" << w[2] << ";\n"
  << "initialRot[2][1]=" << w[5] << ";\n"
  << "initialRot[2][2]=" << myZ[2] << ";\n"


  << "firstscale="   << w[6]<< ";\n"
  << "firstx=" << w[7]<< ";\n"
  << "firsty=" << w[8]<< ";\n\n";
}
