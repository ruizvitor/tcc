//Esqueleto do render de obj
#include <iostream>
#include "camera.h"
#include "helper.h"
#include "ShaderUniforms.h"
#include "GeometryData.h"
#include "createOBJ.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

glm::mat4 P,V,M;
cameraSystem cam;   // the camera
GLuint backgroundTex;
ShaderUniforms TexturePhongShader; // the shaders
GeometryData geometryOBJ;
const char* path ="../models/myjoel.obj";

void initGL()
{
  glClearColor(0,0,0,0);   // set the clear color to black
  glEnable(GL_DEPTH_TEST); // turn on the depth test
  //glEnable(GL_CULL_FACE);  // turn on backface culling

  //glEnable(GL_LINE_SMOOTH);
  //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  // set the camera:
  glm::vec3 eye = glm::vec3( cam.position);
  glm::vec3 center= glm::vec3( cam.position + cam.viewDir);
  glm::vec3 up= glm::vec3( cam.upDir);
  V = glm::lookAt(eye,center,up);

  // create the shaders (the functions are defined in helper.h)
  createProgram_VF("PhongTex_VS.glsl","PhongTex_FS.glsl",&TexturePhongShader.Shader);

  // ***********************************************************************************
  // You can add texture creation from previous assignment
  // GLuint textureID[4];
  // glGenTextures(4, textureID);
  //
  // MyImage earth;
  // earthTex=textureID[0];
  // earth.load("../data/earth.jpg");
  // initTexture(earthTex,1000,500,earth.pixels);
  //
  // MyImage moon;
  // moonTex=textureID[1];
  // moon.load("../data/moon.jpg");
  // initTexture(moonTex,1000,500,moon.pixels);
  //    // Create and bind a BO for vertex data
  // MyImage saturn;
  // saturnTex=textureID[2];
  // saturn.load("../data/saturn.jpg");
  // initTexture(saturnTex,1800,900,saturn.pixels);
  //
  // MyImage background;
  // backgroundTex=textureID[3];
  // background.load("../data/background.jpg");
  // initTexture(backgroundTex,512,512,background.pixels);
  // ***********************************************************************************


  // the space shuttle & other geometry:
  createOBJ(geometryOBJ,path);
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Sphere
  glUseProgram(TexturePhongShader.Shader);

  // create Matrices
  M = glm::mat4();
  glm::vec4 LightSource = V * glm::vec4(0,0,0,1)  ;
  glm::vec4 Color (0.9,0.9,0, 1); // set the color to yellow

  TexturePhongShader.bindUniforms(M,V,P, LightSource, Color,0, 0);

  //Draw
  // 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, geometryOBJ.vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, geometryOBJ.uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, geometryOBJ.normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometryOBJ.elementbuffer);

	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		geometryOBJ.indices.size(),    // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
	);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

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

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitWindowSize(1042, 1024);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

  glutCreateWindow("Cena");
  GLenum err = glewInit();

  glutDisplayFunc(display);
  //glutKeyboardFunc(keyboard);
  //glutMotionFunc(onMouseMove);
  //glutMouseFunc(onMouseDown);
  glutReshapeFunc(reshape);
  //glutIdleFunc(onIdle);
  initGL();
  glutMainLoop();
  return err;
}
