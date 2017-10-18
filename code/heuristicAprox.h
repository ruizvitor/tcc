#ifndef _HEUR_H_
#define _HEUR_H_
#include "Macros.h"

void insertionFancy(float* fx,float value, float *face, float* x,int n)
{
  int j=-1;
  int nsum=n+1;
  for(int i=0;i<(nsum);i++)
  {
    if(value<fx[i])
    {
      for(int j=n;j>i;j--)
      {
        fx[j]=fx[j-1];
        for(int k=0;k<n;k++)
        {
          x[(j*n)+k]=x[((j-1)*n)+k];
        }
      }
      fx[i]=value;
      j=i;
      break;
    }
    else
    {
      if(value==fx[i])
        break;
    }
  }


  if(j!=-1)
  {
    for(int k=0;k<n;k++)
    {
      x[(j*n)+k]=face[k];
    }
  }
}

void boxfocal(  point original,
              GLuint myFrameBuffer,
              Shader & simpleShader,
              Model & ourModel,
              glm::mat4 M,
              glm::mat4 V,
              glm::mat4 P,
              glm::mat4 initialT,
              glm::mat4 initialRot,
              glm::mat4 &initialS,
              int width,
              int height,
              std::vector< unsigned char >& frameImage
            )
{
  glm::mat4 T;
  glm::mat4 R;
  glm::mat4 S;

  float initscalex=initialS[0][0];
  float initscaley=initialS[1][1];
  float initscalez=initialS[2][2];

  point rect;

  float sx_actual;
  float sy_actual;
  float sx_target;
  float sy_target;
  float scale_x;
  float scale_y;
  // glm::mat4 MVP= P * V * M;
  // glm::mat4 invMVP=glm::inverse(MVP);

  // glm::vec4 worldcenter=glm::vec4(0.0f,0.0f,0.0f,1.0f);
  // worldcenter=MVP*worldcenter;
  // float worldcenter_w=worldcenter[3];


  //*********Scale**************************************************************

  // //Bind FrameBuffer
  glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);

  glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  simpleShader.Use();

  //Transformations (Translate*Rotate*Scale)
  T = initialT;
  R = initialRot;
  S = initialS;
  M = T*R*S;

  simpleShader.BindMatrices(&M,&V,&P);
  ourModel.Draw(simpleShader);
  frameImage = GetFrame(width, height);
  // SaveFrameN(frameImage,width,height,22 );

  getBoundingBoxColor(frameImage,rect,height,width);

  sx_target=((float)original.x-(float)original.centerx)/(float)original.centerx;
  sy_target=((float)original.y-(float)original.centery)/(float)original.centery;

  sx_actual=((float)rect.x-(float)rect.centerx ) / (float)rect.centerx;
  sy_actual=((float)rect.y-(float)rect.centery ) / (float)rect.centery;

  cout << "sx_target="<< sx_target << " sy_target="<< sy_target << endl;
  cout << "sx_actual="<< sx_actual << " sy_actual="<< sy_actual << endl;

  scale_x=(sx_target/(sx_actual))*initscalex;
  scale_y=(sy_target/(sy_actual))*initscaley;
  // scale_x=(sx_target/(sx_actual));
  // scale_y=(sy_target/(sy_actual));
  // cout << "scale="<< scale_x << " " << scale_y << endl;
  // initscalex=scale_x;
  // initscaley=scale_y;
  initscalez=(scale_x+scale_y)/2;
  initscalex=initscalez;
  initscaley=initscalez;

  //****************************************************************************
  glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  simpleShader.Use();

  //Transformations (Translate*Rotate*Scale)
  T = initialT;
  R = initialRot;
  S = glm::mat4(1);
  S = glm::scale(S, glm::vec3(initscalex,initscaley,initscalez) );//Scale
  M=T*R*S;

  simpleShader.BindMatrices(&M,&V,&P);
  ourModel.Draw(simpleShader);

  frameImage = GetFrame(width, height);
  // SaveFrameN(frameImage,width,height,1 );
  //****************************************************************************

  initialS[0][0]=initscalex;
  initialS[1][1]=initscaley;
  initialS[2][2]=initscalez;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void paramsAprox( float* fw,
                  float* w,
                  GLuint myFrameBuffer,
                  Shader & simpleShader,
                  Model & ourModel,
                  glm::mat4 M,
                  glm::mat4 & V,
                  glm::mat4 & P,
                  double favg,
                  double cff,
                  Mat & imageGray,
                  glm::mat4 & initialT,
                  glm::mat4 & initialR,
                  glm::mat4 & initialS,
                  int width,
                  int height,
                  std::vector< unsigned char >& frameImage,
                  float near,
                  Mat& mask,
                  point original
                )
{
  glm::mat4 rotMat;
  float distance=1.0;
  float tempVec[N_DIMENSIONS];
  // float min_actual=2.0f;

  int signValue=1;
  cout<<"-- rotations --"<<endl;
  for (int sign = 0; sign < 2; sign++)
  {
    signValue*=-1;
    cout<<"-- "<<signValue<<" --"<<endl;
    // min_actual=2.0f;
    for (int i = 0; i < ANGLE_ITER; i++)
    {
      for (int j = 0; j < ANGLE_ITER; j++)
      {
        for (int z = 0; z < ANGLE_ITER; z++)
        {

          rotMat = glm::rotate(initialR, glm::radians(signValue*ANGLE_DIFF*i), glm::vec3(1.0f,0.0f,0.0f));//Rotate
          rotMat = glm::rotate(rotMat, glm::radians(signValue*ANGLE_DIFF*j), glm::vec3(0.0f,1.0f,0.0f));//Rotate
          rotMat = glm::rotate(rotMat, glm::radians(signValue*ANGLE_DIFF*z), glm::vec3(0.0f,0.0f,1.0f));//Rotate

          M=initialT*rotMat*initialS;
          // boxfocal(original,
          //       myFrameBuffer,
          //       simpleShader,
          //       ourModel,
          //       M,
          //       V,
          //       P,
          //       initialT,
          //       rotMat,
          //       initialS,
          //       width,
          //       height,
          //       frameImage
          //     );
          render(
                  myFrameBuffer,
                  simpleShader,
                  ourModel,
                  M,
                  V,
                  P,
                  width,
                  height,
                  frameImage
                );


          // ApplyMask(mask,frameImage);
          distance=Loss(imageGray, frameImage, favg,cff);

          cout << distance << endl;

          // if(distance<min_actual)
          // {
            // min_actual=distance;
            // rotMat=rotMat*initialS;
            tempVec[0]=rotMat[0][0];
            tempVec[1]=rotMat[0][1];
            tempVec[2]=rotMat[0][2];

            tempVec[3]=rotMat[1][0];
            tempVec[4]=rotMat[1][1];
            tempVec[5]=rotMat[1][2];

            tempVec[6]=near;
          // }
          // insertionFancy(fw,min_actual,tempVec,w,N_DIMENSIONS);
          insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);

        }
      }
    }
  }
}

void paramsAproxFocal( float* fw,
                  float* w,
                  GLuint myFrameBuffer,
                  Shader & simpleShader,
                  Model & ourModel,
                  glm::mat4 M,
                  glm::mat4 & V,
                  glm::mat4 & P,
                  double favg,
                  double cff,
                  Mat & imageGray,
                  glm::mat4 & initialT,
                  glm::mat4 & initialR,
                  glm::mat4 & initialS,
                  int width,
                  int height,
                  std::vector< unsigned char >& frameImage,
                  float near,
                  Mat& mask,
                  point original
                )
{
  glm::mat4 rotMat;
  float distance=1.0;
  float tempVec[N_DIMENSIONS];

  float f=near;
  M=initialT*rotMat*initialS;
  float tempnear=(10*width)/near;
  float step=tempnear*0.1;

  for (int i = 0; i < 10; i++)
  {
    tempnear-=step;
    f=(10*width)/tempnear;
    cout<<"f="<<f<<endl;
    reshape(width,height,P,f);
    render(
            myFrameBuffer,
            simpleShader,
            ourModel,
            M,
            V,
            P,
            width,
            height,
            frameImage
          );

    distance=Loss(imageGray, frameImage, favg,cff);

    cout << distance << endl;

    tempVec[0]=rotMat[0][0];
    tempVec[1]=rotMat[0][1];
    tempVec[2]=rotMat[0][2];

    tempVec[3]=rotMat[1][0];
    tempVec[4]=rotMat[1][1];
    tempVec[5]=rotMat[1][2];

    tempVec[6]=f;

    insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);
  }


}


// void paramsAprox(float* fw,
//               float* w,
//               GLuint myFrameBuffer,
//               Shader & simpleShader,
//               Model & ourModel,
//               glm::mat4 & M,
//               glm::mat4 & V,
//               glm::mat4 & P,
//               double favg,
//               double cff,
//               Mat & imageGray,
//               glm::mat4 initialRot,
//               float& initscalex,
//               float& initscaley,
//               float& initscalez,
//               int width,
//               int height,
//               std::vector< unsigned char >& frameImage,
//               float focal,
//               point original,
//               Mat& mask
//             )
// {
//   M = glm::mat4(1);//Init
//
//   glm::mat4 rotMat;
//   float distance=1.0;
//   float tempVec[N_DIMENSIONS];
//   float f=focal;
//   float tmp=focal*0.01;
//   float min_actual=2.0f;
//
//   // cout << initialRot[0][0] << " " << initialRot[0][1] <<" " << initialRot[0][2] << " " << initialRot[0][3] << endl;
//   // cout << initialRot[1][0] << " " << initialRot[1][1] <<" " << initialRot[1][2] << " " << initialRot[1][3] << endl;
//   // cout << initialRot[2][0] << " " << initialRot[2][1] <<" " << initialRot[2][2] << " " << initialRot[2][3] << endl;
//   // cout << initialRot[3][0] << " " << initialRot[3][1] <<" " << initialRot[3][2] << " " << initialRot[3][3] << endl;
//
//   cout<<"--paramsAprox--"<<endl;
//
//   for(int b=0;b<90;b++)
//   {
//     f=focal-(tmp*b);
//     cout << "focal= " << f << endl;
//     reshape(width,height,P,f);
//
//     boxfocal(original,
//           myFrameBuffer,
//           simpleShader,
//           ourModel,
//           M,
//           V,
//           P,
//
//           initialRot,
//           initscalex,
//           initscaley,
//           initscalez,
//           width,
//           height,
//           frameImage);
//
//     // ApplyMask(mask,frameImage);
//     distance=Loss(imageGray, frameImage, favg,cff);
//
//     cout << distance << endl;
//
//
//     if(distance<min_actual)
//     {
//       min_actual=distance;
//       tempVec[0]=rotMat[0][0];
//       tempVec[1]=rotMat[0][1];
//       tempVec[2]=rotMat[0][2];
//
//       tempVec[3]=rotMat[1][0];
//       tempVec[4]=rotMat[1][1];
//       tempVec[5]=rotMat[1][2];
//
//       tempVec[6]=f;
//     }
//
//     // tempVec[0]=initialRot[0][0];
//     // tempVec[1]=initialRot[0][1];
//     // tempVec[2]=initialRot[0][2];
//     //
//     // tempVec[3]=initialRot[1][0];
//     // tempVec[4]=initialRot[1][1];
//     // tempVec[5]=initialRot[1][2];
//     //
//     // tempVec[6]=f;
//     //
//     // insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);
//   }
//   insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);
//
//   f=w[6];
//   int signValue=1;
//   // for (int k = 0; k < 3; k++)
//   // {
//     cout<<"-- rotations --"<<endl;
//     for (int sign = 0; sign < 2; sign++)
//     {
//       signValue*=-1;
//       cout<<"-- "<<signValue<<" --"<<endl;
//       min_actual=2.0f;
//       for (int i = 0; i < ANGLE_ITER; i++)
//       {
//         for (int j = 0; j < ANGLE_ITER; j++)
//         {
//           for (int z = 0; z < ANGLE_ITER; z++)
//           {
//             reshape(width,height,P,f);
//
//             rotMat = glm::rotate(initialRot, glm::radians(signValue*ANGLE_DIFF*i), glm::vec3(1.0f,0.0f,0.0f));//Rotate
//             rotMat = glm::rotate(rotMat, glm::radians(signValue*ANGLE_DIFF*j), glm::vec3(0.0f,1.0f,0.0f));//Rotate
//             rotMat = glm::rotate(rotMat, glm::radians(signValue*ANGLE_DIFF*z), glm::vec3(0.0f,0.0f,1.0f));//Rotate
//
//
//
//             // if(k==0)
//             // {
//             //   rotMat = glm::rotate(initialRot, glm::radians(signValue*ANGLE_DIFF*i), glm::vec3(1.0f,0.0f,0.0f));//Rotate
//             // }
//             // if(k==1)
//             // {
//             //   rotMat = glm::rotate(initialRot, glm::radians(signValue*ANGLE_DIFF*i), glm::vec3(0.0f,1.0f,0.0f));//Rotate
//             // }
//             // if(k==2)
//             // {
//             //   rotMat = glm::rotate(initialRot, glm::radians(signValue*ANGLE_DIFF*i), glm::vec3(0.0f,0.0f,1.0f));//Rotate
//             // }
//
//             boxfocal(original,
//                     myFrameBuffer,
//                     simpleShader,
//                     ourModel,
//                     M,
//                     V,
//                     P,
//                     rotMat,
//                     initscalex,
//                     initscaley,
//                     initscalez,
//                     width,
//                     height,
//                     frameImage);
//
//             // ApplyMask(mask,frameImage);
//             distance=Loss(imageGray, frameImage, favg,cff);
//
//             cout << distance << endl;
//
//             // if(distance<min_actual)
//             // {
//               min_actual=distance;
//               tempVec[0]=rotMat[0][0];
//               tempVec[1]=rotMat[0][1];
//               tempVec[2]=rotMat[0][2];
//
//               tempVec[3]=rotMat[1][0];
//               tempVec[4]=rotMat[1][1];
//               tempVec[5]=rotMat[1][2];
//
//               tempVec[6]=f;
//             // }
//             // insertionFancy(fw,min_actual,tempVec,w,N_DIMENSIONS);
//             insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);
//
//           }
//           // insertionFancy(fw,min_actual,tempVec,w,N_DIMENSIONS);
//         }
//         // insertionFancy(fw,min_actual,tempVec,w,N_DIMENSIONS);
//       }
//       // insertionFancy(fw,min_actual,tempVec,w,N_DIMENSIONS);
//     }
//   // }
//   // insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);
//
//
//   cout<<"--paramsAprox--"<<endl;
//
//   tempVec[0]=w[0];
//   tempVec[1]=w[1];
//   tempVec[2]=w[2];
//
//   tempVec[3]=w[3];
//   tempVec[4]=w[4];
//   tempVec[5]=w[5];
//
//   tempVec[6]=w[6];
//   calculateZ(tempVec,rotMat);
//   // cout << rotMat[0][0] << " " << rotMat[0][1] <<" " << rotMat[0][2] << " " << rotMat[0][3] << endl;
//   // cout << rotMat[1][0] << " " << rotMat[1][1] <<" " << rotMat[1][2] << " " << rotMat[1][3] << endl;
//   // cout << rotMat[2][0] << " " << rotMat[2][1] <<" " << rotMat[2][2] << " " << rotMat[2][3] << endl;
//   // cout << rotMat[3][0] << " " << rotMat[3][1] <<" " << rotMat[3][2] << " " << rotMat[3][3] << endl;
//
//   //DEBUG
//   reshape(width,height,P,w[6]);
//   boxfocal(original,
//         myFrameBuffer,
//         simpleShader,
//         ourModel,
//         M,
//         V,
//         P,
//         rotMat,
//         initscalex,
//         initscaley,
//         initscalez,
//         width,
//         height,
//         frameImage);
//
//   ApplyMask(mask,frameImage);
//   SaveFrameN(frameImage,width,height,777 );
// }


// void cubeAprox(float* fw,
//               float* w,
//               GLuint myFrameBuffer,
//               Shader & simpleShader,
//               Model & ourModel,
//               glm::mat4 & M,
//               glm::mat4 & V,
//               glm::mat4 & P,
//               double favg,
//               double cff,
//               Mat & imageGray,
//               float firstscale,
//               float firstx,
//               float firsty,
//               glm::mat4 initialRot
//             )
// {
//   cout<<"--cubeAprox--"<<endl;
//
//   glm::mat4 rotMat;
//   float distance=1.0;
//   float tempVec[N_DIMENSIONS];
//   std::vector< unsigned char > frameImage;
//
//   //Bind FrameBuffer
//   glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
//   /* Render here */
//   // Clear the colorbuffer
//   glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
//   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//
//   simpleShader.Use();
//
//   //Transformations (Translate*Rotate*Scale)
//   M = glm::mat4(1);//Init
//   // M = glm::translate(M, glm::vec3(firstx,firsty,0.0f));//Translate
//   rotMat=initialRot;
//   // rotMat = glm::rotate(rotMat, glm::radians(-15.0f*k), glm::vec3(0.0f,0.0f,1.0f));//Rotate
//   // rotMat = glm::rotate(rotMat, glm::radians(-15.0f*i), glm::vec3(0.0f,1.0f,0.0f));//Rotate
//   // rotMat = glm::rotate(rotMat, glm::radians(-15.0f*j), glm::vec3(1.0f,0.0f,0.0f));//Rotate
//   // M = M * rotMat;
//   M = glm::scale(M, glm::vec3(firstscale,firstscale,firstscale));//Scale
//
//   simpleShader.BindMatrices(&M,&V,&P);
//   ourModel.Draw(simpleShader);
//   frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
//   point rect;
//   getBoundingBoxColor(frameImage,rect,WINDOW_HEIGHT,WINDOW_WIDTH);
// std::cout << "diffx=" << rect.x << " diffy=" << rect.y << std::endl;
//   // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 777 );
//
//   /* unbind frameBuffer */
//   glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//   for(int i=0;i<3;i++)
//   {
//     for(int j=0;j<3;j++)
//     {
//       for(int k=0;k<3;k++)
//       {
//         //Bind FrameBuffer
//         glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
//         /* Render here */
//         // Clear the colorbuffer
//         glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//
//         simpleShader.Use();
//
//         //Transformations (Translate*Rotate*Scale)
//         M = glm::mat4(1);//Init
//         M = glm::translate(M, glm::vec3(firstx,firsty,0.0f));//Translate
//         rotMat = initialRot;
//         rotMat = glm::rotate(rotMat, glm::radians(-15.0f*k), glm::vec3(0.0f,0.0f,1.0f));//Rotate
//         rotMat = glm::rotate(rotMat, glm::radians(-15.0f*i), glm::vec3(0.0f,1.0f,0.0f));//Rotate
//         rotMat = glm::rotate(rotMat, glm::radians(-15.0f*j), glm::vec3(1.0f,0.0f,0.0f));//Rotate
//         M = M * rotMat;
//         M = glm::scale(M, glm::vec3(firstscale,firstscale,firstscale));//Scale
//
//         simpleShader.BindMatrices(&M,&V,&P);
//         // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, (100*i)+(10*j)+k );
//         ourModel.Draw(simpleShader);
//         frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
//         distance=Loss(imageGray, frameImage, favg,cff);
//
//         cout << distance << endl;
//
//         tempVec[0]=rotMat[0][0];
//         tempVec[1]=rotMat[1][0];
//         tempVec[2]=rotMat[2][0];
//
//         tempVec[3]=rotMat[0][1];
//         tempVec[4]=rotMat[1][1];
//         tempVec[5]=rotMat[2][1];
//
//         tempVec[6]=firstscale;
//         tempVec[7]=firstx;
//         tempVec[8]=firsty;
//         insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);
//
//         /* unbind frameBuffer */
//         glBindFramebuffer(GL_FRAMEBUFFER, 0);
//       }
//     }
//   }
//
//   for(int i=0;i<3;i++)
//   {
//     for(int j=0;j<3;j++)
//     {
//       for(int k=0;k<3;k++)
//       {
//         //Bind FrameBuffer
//         glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
//         /* Render here */
//         // Clear the colorbuffer
//         glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//
//         simpleShader.Use();
//
//         //Transformations (Translate*Rotate*Scale)
//         M = glm::mat4(1);//Init
//         M = glm::translate(M, glm::vec3(firstx,firsty,0.0f));//Translate
//         rotMat = initialRot;
//         rotMat = glm::rotate(rotMat, glm::radians(15.0f*k), glm::vec3(0.0f,0.0f,1.0f));//Rotate
//         rotMat = glm::rotate(rotMat, glm::radians(15.0f*i), glm::vec3(0.0f,1.0f,0.0f));//Rotate
//         rotMat = glm::rotate(rotMat, glm::radians(15.0f*j), glm::vec3(1.0f,0.0f,0.0f));//Rotate
//         M = M * rotMat;
//         M = glm::scale(M, glm::vec3(firstscale,firstscale,firstscale));//Scale
//
//         simpleShader.BindMatrices(&M,&V,&P);
//
//         ourModel.Draw(simpleShader);
//         // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 1000+(100*i)+(10*j)+k );
//         frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
//         distance=Loss(imageGray, frameImage, favg,cff);
//
//         cout << distance << endl;
//
//         tempVec[0]=rotMat[0][0];
//         tempVec[1]=rotMat[1][0];
//         tempVec[2]=rotMat[2][0];
//
//         tempVec[3]=rotMat[0][1];
//         tempVec[4]=rotMat[1][1];
//         tempVec[5]=rotMat[2][1];
//
//         tempVec[6]=firstscale;
//         tempVec[7]=firstx;
//         tempVec[8]=firsty;
//         insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);
//
//         /* unbind frameBuffer */
//         glBindFramebuffer(GL_FRAMEBUFFER, 0);
//       }
//     }
//   }
// }
//

#endif // _HEUR_H_
