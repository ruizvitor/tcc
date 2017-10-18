#include "heuristic.hpp"

void boxfocal(  point original,
                point &rect,
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

  // point rect;

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
  // cout << "myFrameBuffer.fbo="<< myFrameBuffer << endl;
  // glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

  // sx_target=((float)original.x-(float)original.centerx)/(float)original.centerx;
  // sy_target=((float)original.y-(float)original.centery)/(float)original.centery;
  //
  // sx_actual=((float)rect.x-(float)rect.centerx ) / (float)rect.centerx;
  // sy_actual=((float)rect.y-(float)rect.centery ) / (float)rect.centery;

  sx_target=((float)original.x-(float)original.centerx);
  sy_target=((float)original.y-(float)original.centery);

  sx_actual=((float)rect.x-(float)rect.centerx );
  sy_actual=((float)rect.y-(float)rect.centery );

  // cout << endl<<endl << "width="<<width<<" height="<<height<<endl;
  // cout << "original.x="<< original.x << " original.y="<< original.y << endl;
  // cout << "original.centerx="<< original.centerx << " original.centery="<< original.centery << endl;
  //
  // cout << "rect.x="<< rect.x << " rect.y="<< rect.y << endl;
  // cout << "rect.centerx="<< rect.centerx << " rect.centery="<< rect.centery << endl;
  //
  cout << "sx_target="<< sx_target << " sy_target="<< sy_target << endl;
  cout << "sx_actual="<< sx_actual << " sy_actual="<< sy_actual << endl;

  scale_x=(sx_target/(sx_actual))*initscalex;
  scale_y=((sy_target/(sy_actual))*initscaley);
  // scale_x=(sx_target/(sx_actual));
  // scale_y=(sy_target/(sy_actual));
  // cout<<"scale_x="<<scale_x<<" scale_y="<<scale_y<<endl;
  // scale_x=(sx_target/(sx_actual));
  // scale_y=(sy_target/(sy_actual));
  // cout << "scale="<< scale_x << " " << scale_y << endl;
  // initscalex=scale_x;
  // initscaley=scale_y;
  initscalez=(scale_x+scale_y)/2;
  initscalex=initscalez;
  initscaley=initscalez;
  initialS[0][0]=initscalex;
  initialS[1][1]=initscaley;
  initialS[2][2]=initscalez;

  //****************************************************************************
  glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  simpleShader.Use();

  //Transformations (Translate*Rotate*Scale)
  T = initialT;
  R = initialRot;
  S = initialS;
  // S = glm::scale(S, glm::vec3(initscalex,initscaley,initscalez) );//Scale
  M=T*R*S;

  simpleShader.BindMatrices(&M,&V,&P);
  ourModel.Draw(simpleShader);
  frameImage = GetFrame(width, height);
  getBoundingBoxColor(frameImage,rect,height,width);

  sx_target=((float)original.x-(float)original.centerx);
  sy_target=((float)original.y-(float)original.centery);

  sx_actual=((float)rect.x-(float)rect.centerx );
  sy_actual=((float)rect.y-(float)rect.centery );

  cout << "sx_target="<< sx_target << " sy_target="<< sy_target << endl;
  cout << "sx_actual="<< sx_actual << " sy_actual="<< sy_actual << endl;

  // cout<<"scale_x="<<scale_x<<" scale_y="<<scale_y<<endl;

  // frameImage = GetFrame(width, height);
  // SaveFrameN(frameImage,width,height,1 );
  //****************************************************************************

  // initialS[0][0]=initscalex;
  // initialS[1][1]=initscaley;
  // initialS[2][2]=initscalez;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
