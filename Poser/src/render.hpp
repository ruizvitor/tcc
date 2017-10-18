#ifndef _RENDER_H_
#define _RENDER_H_

#include <iostream>
#include <math.h>

#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
// Image Handler
#include <opencv2/opencv.hpp>

#include "createShader.hpp"
#include "model.hpp"

using namespace std;

class ScreenQuad
{
  public:
    GLuint quadVAO;
    GLuint quadVBO;
    ScreenQuad();   // This is the constructor declaration
    ~ScreenQuad();  // This is the destructor: declaration
};

class FrameBuffer
{
  public:
    GLuint FBO;
    GLuint Tex;
    GLuint Tex2;
    GLuint Depth;
    FrameBuffer(unsigned int width, unsigned int height);   // This is the constructor declaration
    ~FrameBuffer();  // This is the destructor: declaration
  private:
    void CHECK_FRAMEBUFFER_STATUS();
};

std::vector< unsigned char > GetFrame(unsigned int w,unsigned int h);

void mylaplacian(Mat& src,unsigned int w,unsigned int h);

void updateCamera(glm::mat4& V);
void reshape(int w, int h, glm::mat4& P, float near);
void CHECK_FrameBuffer_STATUS();
void GenerateFBO(GLuint* FBO,unsigned int width, unsigned int height);
void render(
          FrameBuffer & myFrameBuffer,
          Shader & texShader,
          Shader & simpleShader,
          Shader & screenShader,
          Model & ourModel,
          glm::mat4 &M,
          glm::mat4 &V,
          glm::mat4 &P,
          int &width,
          int &height,
          Mat &image,
          ScreenQuad &squad
      );

#endif // _RENDER_H_
