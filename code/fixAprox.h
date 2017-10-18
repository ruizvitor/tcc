void paramsAprox(float* fw,
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
              glm::mat4 initialRot,
              float& initscalex,
              float& initscaley,
              float& initscalez,
              int width,
              int height,
              std::vector< unsigned char >& frameImage
            )
{
  cout<<"--paramsAprox--"<<endl;
  M = glm::mat4(1);//Init
  glm::mat4 T=glm::mat4(1);
  glm::mat4 R=glm::mat4(1);
  glm::mat4 S=glm::mat4(1);
  glm::mat4 rotMat;
  float distance=1.0;
  float tempVec[N_DIMENSIONS];

  frameImage = GetFrame(width, height);
  distance=Loss(imageGray, frameImage, favg,cff);

  cout << distance << endl;

  tempVec[0]=rotMat[0][0];
  tempVec[1]=rotMat[1][0];
  tempVec[2]=rotMat[2][0];

  tempVec[3]=rotMat[0][1];
  tempVec[4]=rotMat[1][1];
  tempVec[5]=rotMat[2][1];

  tempVec[6]=f;

  insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);

  // for(int k=1;k<5;k++)
  // {
  //   rotMat = glm::rotate(initialRot, glm::radians(-15.0f*k), glm::vec3(0.0f,0.0f,1.0f));//Rotate
  //   // rotMat = glm::rotate(initialRot, glm::radians(-15.0f*k), glm::vec3(0.0f,0.0f,1.0f));//Rotate
  //   // rotMat = glm::rotate(initialRot, glm::radians(-15.0f*k), glm::vec3(0.0f,0.0f,1.0f));//Rotate
  //
  //   render();
  //
  //   frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  //   distance=Loss(imageGray, frameImage, favg,cff);
  //
  //   cout << distance << endl;
  //
  //   tempVec[0]=rotMat[0][0];
  //   tempVec[1]=rotMat[1][0];
  //   tempVec[2]=rotMat[2][0];
  //
  //   tempVec[3]=rotMat[0][1];
  //   tempVec[4]=rotMat[1][1];
  //   tempVec[5]=rotMat[2][1];
  //
  //   tempVec[6]=f;
  //
  //   insertionFancy(fw,distance,tempVec,w,N_DIMENSIONS);
  // }
}
