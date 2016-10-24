#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include "SOIL.h"
#include <string.h>

void SaveFrame(unsigned int w,unsigned int h,unsigned int i)
{
  std::vector< unsigned char > buf( w * h * 3 );
  glPixelStorei( GL_PACK_ALIGNMENT, 1 );
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
    std::cout << "Error! Unable to create img.bmp" << std::endl;
  }
}

GLuint generateColorTexture(unsigned int width,unsigned int height)
{
  // The texture we're going to render to
  GLuint renderedTexture;
  glGenTextures(1, &renderedTexture);

  // "Bind" the newly created texture : all future texture functions will modify this texture
  glBindTexture(GL_TEXTURE_2D, renderedTexture);

  // Give an empty image to OpenGL ( the last "0" )
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

  // Poor filtering. Needed !
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  //Unbind
  glBindTexture(GL_TEXTURE_2D, 0);
  return renderedTexture;
}

// Generate FBO and two empty textures
void GenerateFBO(GLuint* FBO,unsigned int width, unsigned int height)
{
  glGenFramebuffers(1, FBO);                     //Generate a framebuffer object(FBO)
  glBindFramebuffer(GL_FRAMEBUFFER, *FBO);         // and bind it to the pipeline

  GLuint renderedTexture=generateColorTexture(width, height);//generate empty texture
  // generateDepthTexture(width, height);//generate empty texture

  // bind textures to pipeline. texture_depth is optional .0 is the mipmap level. 0 is the heightest
  // Set "renderedTexture" as our colour attachement #0
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
  // glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture_depth, 0);//optional

  // Set the list of draw buffers.
  GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

  // drawbuffer.push_back(GL_COLOR_ATTACHMENT0 + attachment_index_color_texture);    //add attachements

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){  //Check for FBO completeness
    std::cout << "Error! FrameBuffer is not complete" << std::endl;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);    //unbind framebuffer
}

#endif // _FRAMEBUFFER_H_
