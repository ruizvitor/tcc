#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include "SOIL.h"
#include <string.h>

// dilate()
// {
//
// }

void toGray(cv::Mat &image, cv::Mat &imgray)
{
  uint8_t *img = image.data;
  uint8_t *gray = imgray.data;
  unsigned int n=image.rows;
  unsigned int m=image.cols;
  unsigned long int avg=0;
  //Mean
  for(unsigned i=0;i<n;i++)
  {
    for(unsigned j=0;j<m;j++)
    {
      avg=0;
      for(unsigned k=0;k<3;k++)
      {
        avg+=img[(i*m*3+j*3)+k];
      }
      gray[(i*m+j)]=avg/3;
    }
  }
}

template <typename T1, typename T2, typename T3, typename T4>
// void MapTex( Model &model,
//             std::vector< unsigned char > &frameImage,
//             std::vector< float > &newUV,
//             unsigned int m,
//             unsigned int n)
void MapTex( T1 &model,
            T2 &frameImage,
            T3 &newUV,
            T4 m,
            T4 n)
{
  GLuint textureID=model.textures_loaded[0].id;
  std::cout << "textureID="<< textureID << " path=" << model.textures_loaded[0].path.C_Str() << std::endl;
  glBindTexture(GL_TEXTURE_2D, textureID);
  GLint w = 0;
  GLint h = 0;
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
  // std::vector< unsigned char > buf( w * h * 3,0 );
  // cv::Mat buf(h,w,CV_8UC3,0);
  // cv::Mat buf = cv::Mat::zeros(h,w, CV_8UC3);
  cv::Mat buf = cv::Mat::zeros(h,w, CV_8UC4);
  uint8_t *bufTemp = buf.data;
  // glGetTexImage(GL_TEXTURE_2D,0,GL_RGB,GL_UNSIGNED_BYTE,&buf[0]);
  glBindTexture(GL_TEXTURE_2D, 0);

  std::cout << "w= " << w << "h= " << h << std::endl;


  int u=0;
  int v=0;
  int address=0;


  //Fix Opencv BGR to RGB
  for(unsigned int i=0;i<n;i++)
  {
    for(unsigned int j=0;j<m;j++)
    {
      address=3*((i*m)+j);
      u=newUV[address]*h;
      v=newUV[address+1]*w;

      //RGB
      // buf[ 3*((u*w)+v) ]= frameImage[address+2];//c
      // buf[ 3*((u*w)+v)+1 ]= frameImage[address+1];
      // buf[ 3*((u*w)+v)+2 ]= frameImage[address];

      //OPENCV USE BGRA
      bufTemp[ 4*((u*w)+v) ]= frameImage[address];//center
      bufTemp[ 4*((u*w)+v)+1 ]= frameImage[address+1];
      bufTemp[ 4*((u*w)+v)+2 ]= frameImage[address+2];
      bufTemp[ 4*((u*w)+v)+3 ]= 255;

      //OPENCV USE BGR
      // bufTemp[ 3*((u*w)+v) ]= frameImage[address];
      // bufTemp[ 3*((u*w)+v)+1 ]= frameImage[address+1];
      // bufTemp[ 3*((u*w)+v)+2 ]= frameImage[address+2];

    }
  }

  std::cout << "gonna save" << std::endl;

  cv::Mat image;
  image.Mat::create(h,w, CV_8UC4);
  // cv::Mat image = cv::Mat::zeros(h,w, CV_8UC4);
  uint8_t *imageTemp = image.data;
  flipImgAlpha(bufTemp,imageTemp,w,h);//OPENGL INVERT Y
  imwrite( "out/newSapo.png", image );
  // imwrite( "out/newSapo.png", buf );

  // std::vector< unsigned char > image( w * h * 3 );
  // flipImg(buf,image,w,h);//OPENGL INVERT Y
  //
  // const char* filename = "out/newSapo.bmp";
  // // char const *pchar = model.textures_loaded[0].path.C_Str();
  //
  // int err = SOIL_save_image
  // (
  //   filename,
  //   SOIL_SAVE_TYPE_BMP,
  //   w, h, 3,
  //   &image[0]
  // );
  // if(!err){
  //   std::cout << "Error! Unable to create "<< filename << std::endl;
  // }
  //imwrite
}

std::vector< float > GetNewUV(unsigned int w,unsigned int h)
{
  glReadBuffer(GL_COLOR_ATTACHMENT1);//Render Result
  std::vector< float > buf( w * h * 3 );
  glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
  glReadPixels( 0, 0, w, h, GL_RGB, GL_FLOAT, &buf[0] );

  // for(unsigned int i=0;i<( w * h * 2 );i++)
  // {
  //   if((float)buf[i]!=0.0)
  //     std::cout << (float)buf[i] << " ";
  // }
  return buf;
}

std::vector< unsigned char > GetFrame(unsigned int w,unsigned int h)
{
  glReadBuffer(GL_COLOR_ATTACHMENT0);//Render Result
  std::vector< unsigned char > buf( w * h * 3 );
  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
  glReadPixels( 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, &buf[0] );
  return buf;
}

void SaveFrame(unsigned int w,unsigned int h,unsigned int i)
{
  glReadBuffer(GL_COLOR_ATTACHMENT0);//Render Result
  std::vector< unsigned char > buf( w * h * 3 );
  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
  glReadPixels( 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, &buf[0] );

  std::string s = "out/"+std::to_string(i)+".bmp";
  char const *pchar = s.c_str();

  int err = SOIL_save_image
  (
    pchar,
    SOIL_SAVE_TYPE_BMP,
    w, h, 3,
    &buf[0]
  );
  if(!err){
    std::cout << "Error! Unable to create "<< s << std::endl;
  }
}

void CHECK_FRAMEBUFFER_STATUS()
{
  GLenum status;
  status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
  switch(status)
  {
    case GL_FRAMEBUFFER_COMPLETE:
      break;

    case GL_FRAMEBUFFER_UNSUPPORTED:
      /* choose different formats */
      break;

    default:
    {
      /* programming error; will fail on all hardware */
      std::cout << "Framebuffer Error" << std::endl;
      exit(-1);
    }
  }
}

// Generate FBO and two empty textures
void GenerateFBO(GLuint* FBO,unsigned int width, unsigned int height)
{
  GLuint tempTex;
  GLuint tempTex2;
  GLuint tempDepth;
  glGenFramebuffers(1, FBO);
  glGenTextures(1, &tempTex);
  glGenTextures(1, &tempTex2);
  glGenRenderbuffers(1, &tempDepth);

  glBindFramebuffer(GL_FRAMEBUFFER, *FBO);
  //tex1
  glBindTexture(GL_TEXTURE_2D, tempTex);
  glTexImage2D(   GL_TEXTURE_2D,
                  0,
                  GL_RGB,
                  width, height,
                  0,
                  GL_RGB,
                  GL_UNSIGNED_BYTE,
                  NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //tex2
  glBindTexture(GL_TEXTURE_2D, tempTex2);
  glTexImage2D(   GL_TEXTURE_2D,
                  0,
                  GL_RGB32F,
                  width, height,
                  0,
                  GL_RGB,
                  GL_FLOAT,
                  NULL);

  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tempTex, 0);//mipmap 0
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tempTex2, 0);//mipmap 0

  glBindRenderbuffer(GL_RENDERBUFFER, tempDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
  glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, tempDepth);

  // Set the list of draw buffers.
  GLenum DrawBuffers[2] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, DrawBuffers); // "2" is the size of DrawBuffers

  CHECK_FRAMEBUFFER_STATUS();
}

#endif // _FRAMEBUFFER_H_
