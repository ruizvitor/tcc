//Esqueleto do render de obj
#include <iostream>
#include <vector>
//
#include "frameBuffer.h"
#include "camera.h"
#include "createShader.h"
#include "model.h"
#include "SOIL.h"
//
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//
// #include <pthread.h>
// void* simpleFunc(void*) { return NULL; }
// void forcePThreadLink() { pthread_t t1; pthread_create(&t1, NULL, &simpleFunc, NULL); }
//
using namespace std;
//
// Properties
#define WINDOW_HEIGHT 480
#define WINDOW_WIDTH 640
//

void reshape(int w, int h, glm::mat4* P)
{
  glViewport(0,0,(GLsizei) w, (GLsizei) h);
  *P = glm::perspective(70.0f, (GLfloat) w/ (GLfloat) h, 10.0f, 800.0f);
}

void updateCamera(cameraSystem *cam,glm::mat4* V)
{
  glm::vec3 eye = glm::vec3( cam->position);
  glm::vec3 center= glm::vec3( cam->position + cam->viewDir);
  glm::vec3 up= glm::vec3( cam->upDir);
  *V = glm::lookAt(eye,center,up);
}

int main(void)
{
  //Some definitions
  glm::mat4 P,V,M; // Projection, View, Model Matrices
  cameraSystem cam;   // the camera
  //

  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit())
  {
    return -1;
  }
  //
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  //
  //Disable resize
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Scene", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  // Initialize GLEW to setup the OpenGL Function pointers
  glewExperimental = GL_TRUE;
  glewInit();

  //Calculate Projection Matrix
  reshape(WINDOW_WIDTH, WINDOW_HEIGHT, &P);

  // Setup some OpenGL options
  glEnable(GL_DEPTH_TEST);

  //Initialize Shader
  Shader simpleShader("shaders/simple_VS.glsl","shaders/simple_FS.glsl");
  //Initialize Model
  Model ourModel("../models/myjoel2.obj");

  //Initialize Model Matrix
  glm::vec4 lightPos=glm::vec4(-10.0,10.0,-10.0,1.0);
  // glm::vec4 lightPos=glm::vec4(-10.0,10.0,100.0,1.0);

  //Create FrameBuffer
  GLuint myFrameBuffer;
  GenerateFBO(&myFrameBuffer, WINDOW_WIDTH, WINDOW_HEIGHT);

  unsigned int i=0;
  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window))
  {
    /* Poll for and process events */
    glfwPollEvents();

    /* Render here */
    // Clear the colorbuffer
    glClearColor(0.1,0.1,0.1,0);   // set the clear color to black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    simpleShader.Use();
    // Transformation matrices
    M = glm::mat4(1);//Translate
    // M = glm::rotate(M, glm::radians(90.0f*i), glm::vec3(0.f,1.f,0.f));//Rotate
    M = glm::rotate(M, glm::radians(0.1f*i), glm::vec3(0.f,1.f,0.f));//Rotate
    M = glm::scale(M, glm::vec3(15.0f,15.0f,15.0f));//Scale
    //Calculate View Matrix
    updateCamera(&cam,&V);
    simpleShader.BindMatrices(&M,&V,&P,&lightPos);
    // glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
    ourModel.Draw(simpleShader);
    // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, i);
    i++;
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Sleep for test
    // glfwWaitEventsTimeout(10);
    /* Swap front and back buffers */
    glfwSwapBuffers(window);
    //Force End
    // if(i>4)
    //   glfwSetWindowShouldClose(window, 1);
  }

  glfwTerminate();
  return 0;
}
