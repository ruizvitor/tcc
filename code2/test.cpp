//Esqueleto do render de obj
#include <iostream>
#include <vector>
//
#include "camera.h"
#include "createShader.h"
#include "model.h"
#include "SOIL.h"
//
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//
#include <pthread.h>
void* simpleFunc(void*) { return NULL; }
void forcePThreadLink() { pthread_t t1; pthread_create(&t1, NULL, &simpleFunc, NULL); }
//
using namespace std;
//
glm::mat4 P,V,M;
cameraSystem cam;   // the camera
GLuint shader;
GLuint MatrixID;
//
void initGL()
{
  glClearColor(0.1,0.1,0.1,0);   // set the clear color to black
  glEnable(GL_DEPTH_TEST); // turn on the depth test
  glEnable(GL_CULL_FACE);  // turn on backface culling

  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  // set the camera:
  glm::vec3 eye = glm::vec3( cam.position);
  glm::vec3 center= glm::vec3( cam.position + cam.viewDir);
  glm::vec3 up= glm::vec3( cam.upDir);
  V = glm::lookAt(eye,center,up);

  // create the shaders (the functions are defined in helper.h)
  Shader shader("shaders/simple_VS.glsl","shaders/simple_FS.glsl");
  Model ourModel("../models/myjoel.obj");
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(shader);

  M = glm::rotate(50.f, glm::vec3(0.f,1.f,0.f));
  M = glm::scale(M, glm::vec3(10.0f,10.0f,10.0f));
  glm::mat4 MVP=P*V*M;
  glm::vec3 LightSource (0,0,0);
  glm::vec4 Color (0.3,0.3,0.3, 1); // set the color to gray
  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

  //Unbind
  glBindVertexArray(0);
  glUseProgram(0);
  glutSwapBuffers();
}

void reshape(int w, int h)
{
  glViewport(0,0,(GLsizei) w, (GLsizei) h);
  P = glm::perspective(70.0f, (GLfloat) w/ (GLfloat) h, 10.0f, 800.0f);
}

void onIdle() {

  // t+= speed;  // increase the time parameter

  glutPostRedisplay();
}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitWindowSize(640, 480);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

  glutCreateWindow("Cena");
  GLenum err = glewInit();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutIdleFunc(onIdle);
  initGL();
  glutMainLoop();
  return err;
}
