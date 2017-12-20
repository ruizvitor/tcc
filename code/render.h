// void reshape(int w, int h, glm::mat4& P, float f)
// {
//   // glViewport(0,0,(GLsizei) w, (GLsizei) h);
//   float newH=((float)(h)/2);
//   float newW=((float)(w)/2);
//   newH=newH/1000;
//   newW=newW/1000;
//
//
//   // if(h>=1000)
//   // {
//   //   newH=newH/1000;
//   // }
//   // if(w>=1000)
//   // {
//   //   newW=newW/1000;
//   // }
//   // if((h>=100)&&(h<1000))
//   // {
//   //   newH=newH/100;
//   // }
//   // if((w>=100)&&(w<1000))
//   // {
//   //   newW=newW/100;
//   // }
//
//
//   // *P = glm::frustum(-newW, newW, -newH, newH,10.0f,1000.0f);
//   P = glm::frustum(-newW, newW, -newH, newH,f,1000.0f);
//   // *P = glm::ortho(-newW, newW, -newH, newH,10.0f,1000.0f);
// }

void reshape(int w, int h, glm::mat4& P, float near)
{
  // P = glm::frustum(-1.0f, 1.0f, ((float)h/(float)w), -((float)h/(float)w),near,1000.0f);
  P = glm::frustum(-1.0f, 1.0f, -((float)h/(float)w), ((float)h/(float)w),near,1000.0f);
}

void calculateZ(float* vec, glm::mat4& rotMat)
{
  float normX=sqrt(vec[0]*vec[0]+
                  vec[1]*vec[1]+
                  vec[2]*vec[2]);

  float normY=sqrt(vec[3]*vec[3]+
                  vec[4]*vec[4]+
                  vec[5]*vec[5]);
  vec[0] = vec[0]/normX;
  vec[1] = vec[1]/normX;
  vec[2] = vec[2]/normX;

  vec[3] = vec[3]/normY;
  vec[4] = vec[4]/normY;
  vec[5] = vec[5]/normY;

  glm::vec3 myZ=cross(glm::vec3(vec[0],vec[1],vec[2]),glm::vec3(vec[3],vec[4],vec[5]) );

  // rotMat[0][0]=vec[0]*normX;//x1
  rotMat[0][0]=vec[0];//x1
  rotMat[1][0]=vec[3];//y1
  rotMat[2][0]=-myZ[0];//z1

  rotMat[0][1]=vec[1];//x2
  // rotMat[1][1]=vec[4]*normX;//y2
  rotMat[1][1]=vec[4];//y2
  rotMat[2][1]=-myZ[1];//z2

  rotMat[0][2]=vec[2];//x3
  rotMat[1][2]=vec[5];//y3
  // rotMat[2][2]=-myZ[2]*normX;//z3
  rotMat[2][2]=-myZ[2];//z3
}

void render(
              GLuint myFrameBuffer,
              Shader & simpleShader,
              Model & ourModel,
              glm::mat4 M,
              glm::mat4 V,
              glm::mat4 P,
              int width,
              int height,
              std::vector< unsigned char >& frameImage
            )
{
  //****************************************************************************
  glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);

  glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  simpleShader.Use();

  //Transformations (Translate*Rotate*Scale)

  simpleShader.BindMatrices(&M,&V,&P);
  ourModel.Draw(simpleShader);

  frameImage = GetFrame(width, height);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //****************************************************************************
}
