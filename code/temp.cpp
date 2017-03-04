//Esqueleto do render de obj
#include <iostream>
#include <vector>
//
#include "camera.h"
#include "createShader.h"
#include "model.h"
#include "SOIL.h"
#include "MyImage.h"
#include "lossfunc.h"
#include "pose.h"
#include "frameBuffer.h"
//
#include <cv.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/quaternion.hpp>
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
#define WINDOW_HEIGHT 1024
#define WINDOW_WIDTH 1024
#define MAX_ITER 1000
#define MIN_ERROR 0.00001
//

glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest)
{
	start = normalize(start);
	dest = normalize(dest);

	float cosTheta = dot(start, dest);
	glm::vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f){
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
		if (glm::length2(rotationAxis) < 0.01 ) // bad luck, they were parallel, try again!
			rotationAxis = cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

		rotationAxis = normalize(rotationAxis);
		return glm::angleAxis(180.0f, rotationAxis);
	}

	rotationAxis = cross(start, dest);

	float s = sqrt( (1+cosTheta)*2 );
	float invs = 1 / s;

	return glm::quat(
		s * 0.5f,
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	);

}



void reshape(int w, int h, glm::mat4* P)
{
  glViewport(0,0,(GLsizei) w, (GLsizei) h);
  // FOV (rectilinear) = 2 * arctan (frame size/(focal length * 2))
  *P = glm::perspective(70.0f, (GLfloat) w/ (GLfloat) h, 10.0f, 800.0f);
  *P = glm::scale(*P, glm::vec3(-1.0f,1.0f,1.0f));//Fix UV from frameBuffer
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
  //Show Window
  // window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Scene", NULL, NULL);
  //Hide Window
  window = glfwCreateWindow(1, 1, "Scene", NULL, NULL);
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

  //Images
  Mat image1 = imread("../material/cj_l2.JPG", CV_LOAD_IMAGE_GRAYSCALE);
  // Myimage imgProj("../material/ff0.JPG");
  Mat imageColor = imread("../material/cj_l2.JPG", CV_LOAD_IMAGE_COLOR);
  uint8_t *imgProj = imageColor.data;
  // Mat image1 = imread("../material/frontClean.JPG", CV_LOAD_IMAGE_GRAYSCALE);
  double favg=Mean(image1);
  double cff=MyCff(image1,favg);

  // int colorR = 169;
  // int colorG = 157;
  // int colorB = 205;
  // int thresholdR = 50;
  // int thresholdG = 30;
  // int thresholdB = 0;
  // int gridsize = 20;
  // float vecX[3]={0,0,0};
  // float vecY[3]={0,0,0};//Result of Cross product between x and z, always positive because of photo
  // float vecZ[3]={0,0,0};
  // FindPlane(image1,colorR,colorG,colorB,thresholdR,thresholdG,thresholdB,gridsize,vecX,vecY,vecZ);

  // cam.rightDir	= glm::vec4(vecX[0],vecX[1],vecX[2], 1.0);
  // cam.upDir		= glm::vec4(vecY[0],vecY[1],vecY[2], 1.0);
  // cam.viewDir	= glm::vec4(-vecZ[0],vecZ[1],vecZ[2], 1.0);
  // cam.moveForward(100);
  // std::cout << "{ " << cam.position.x << " , " << cam.position.y << " , " << cam.position.z << " }" << std::endl;

  // Initialize Shader
  // Shader simpleShader("shaders/PhongTex_VS.glsl","shaders/PhongTex_FS.glsl");
  Shader simpleShader("shaders/simple_VS.glsl","shaders/simple_FS.glsl");
  //Initialize Model
  Model ourModel("../models/frogfacecam.obj");
  // Model ourModel("../models/myjoel2.obj");


  //Initialize Light only for phong
  // glm::vec3 lightPos=glm::vec3(-10.0,10.0,10.0);
  // glm::vec4 lightPos=glm::vec4(-10.0,10.0,100.0,1.0);

  //Create FrameBuffer
  GLuint myFrameBuffer;
  GenerateFBO(&myFrameBuffer, WINDOW_WIDTH, WINDOW_HEIGHT);

  M = glm::mat4(1);//Translate
  // M = glm::translate(M, glm::vec3(-1.2f,1.0f,0.0f));//Scale
  // M = glm::rotate(M, glm::radians(73.8f), glm::vec3(1.0f,-0.15f,0.1f));//Rotate
  // M = glm::scale(M, glm::vec3(15.0f,15.0f,15.0f));//Scale
  updateCamera(&cam,&V);


  // // //BEGIN_TEST MahalanobisDistance
  // glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  // // /* Render here */
  // // // Clear the colorbuffer
  // glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // //
  // simpleShader.Use();
  // updateCamera(&cam,&V);
  // //Calculate View Matrix
  // // simpleShader.BindMatrices(&M,&V,&P,&lightPos);
  // simpleShader.BindMatrices(&M,&V,&P);
  //
  // ourModel.Draw(simpleShader);
  // // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 20);
  // //
  // // float vectors[] = { 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  // // // std::vector<float> vectors (arr, arr + sizeof(arr) / sizeof(arr[0]) );
  // //
  // // std::vector< unsigned char > frameImage;
  // // /* unbind frameBuffer */
  // glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //
  // for(unsigned int i=0;i<4;i++)
  // {
  //   // //BEGIN_TEST MahalanobisDistance
  //   glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  //   /* Render here */
  //   // Clear the colorbuffer
  //   glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  //   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //
  //   simpleShader.Use();
  //
  //
  //   M = glm::rotate(M, glm::radians(73.8f), glm::vec3(1.0f,-0.15f,0.1f));//Rotate
  //   M = glm::mat4(1);//Translate
  //   M = glm::translate(M, glm::vec3(vectors[(i*4)],vectors[(i*4)+1],vectors[(i*4)+2]));//Scale
  //   M = glm::scale(M, glm::vec3(15.0f,15.0f,15.0f));//Scale
  //
  //   simpleShader.BindMatrices(&M,&V,&P);
  //
  //   ourModel.Draw(simpleShader);
  //   // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 21);
  //
  //   frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  //   vectors[(i*4)+3]=Loss(image1, frameImage, favg,cff);
  //   cout << vectors[(i*4)+3] << endl;
  //
  //   /* unbind frameBuffer */
  //   glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // }

  // for(unsigned int i=0;i<16;i++)
  // {
  //   cout << vectors[i] << endl;
  // }

  ////TEST CUBE FACES FOR BEST INITIAL pose
  float distance=1.0;
  std::vector< unsigned char > frameImage;
  float last_distance=1.0;
  float var_distance=1.0;

  for(unsigned int i=0;i<4;i++)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
    /* Render here */
    // Clear the colorbuffer
    glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    simpleShader.Use();

    M = glm::mat4(1);//Init
    // M = glm::translate(M, glm::vec3(0.0f,0.0f,0.0f));//Translate
    M = glm::scale(M, glm::vec3(15.0f,15.0f,15.0f));//Scale
    // M = glm::rotate(M, glm::radians(15.0f), glm::vec3(0.0f,0.0f,1.0f));//Rotate
    M = glm::rotate(M, glm::radians(90.0f*i), glm::vec3(0.0f,1.0f,0.0f));//Rotate
    // M = glm::rotate(M, glm::radians(90.0f), glm::vec3(0.0f,0.0f,-1.0f));//Rotate

    simpleShader.BindMatrices(&M,&V,&P);

    ourModel.Draw(simpleShader);
    SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 20+i);
    frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
    distance=Loss(image1, frameImage, favg,cff);
    var_distance=last_distance-distance;
    last_distance=distance;
    cout << distance <<" "<<var_distance << endl;

    /* unbind frameBuffer */
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  for(unsigned int i=1;i<4;i++)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
    /* Render here */
    // Clear the colorbuffer
    glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    simpleShader.Use();

    M = glm::mat4(1);//Init
    // M = glm::translate(M, glm::vec3(0.0f,0.0f,0.0f));//Translate
    M = glm::scale(M, glm::vec3(15.0f,15.0f,15.0f));//Scale
    // M = glm::rotate(M, glm::radians(15.0f), glm::vec3(0.0f,0.0f,1.0f));//Rotate
    // M = glm::rotate(M, glm::radians(15.0f), glm::vec3(1.0f,0.0f,0.0f));//Rotate
    M = glm::rotate(M, glm::radians(90.0f*i), glm::vec3(1.0f,0.0f,0.0f));//Rotate
    // M = glm::rotate(M, glm::radians(90.0f), glm::vec3(0.0f,0.0f,-1.0f));//Rotate

    simpleShader.BindMatrices(&M,&V,&P);

    ourModel.Draw(simpleShader);
    SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 24+i);
    frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
    distance=Loss(image1, frameImage, favg,cff);
    var_distance=last_distance-distance;
    last_distance=distance;
    cout << distance <<" "<<var_distance << endl;

    /* unbind frameBuffer */
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  ////END TEST CUBE FACES

  // float distance=1.0;
  // std::vector< unsigned char > frameImage;
  // float last_distance=1.0;
  // float var_distance=1.0;
  // for(unsigned int i=0;((var_distance>MIN_ERROR)&&(i<MAX_ITER));i++)
  // {
  //   // //BEGIN_TEST MahalanobisDistance
  //   glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  //   /* Render here */
  //   // Clear the colorbuffer
  //   glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  //   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //
  //   simpleShader.Use();
  //
  //   //Calculate View Matrix
  //   // cam.position.z=cam.position.z-0.1;
  //   // updateCamera(&cam,&V);
  //   // simpleShader.BindMatrices(&M,&V,&P,&lightPos);
  //
  //   M = glm::rotate(M, glm::radians(73.8f), glm::vec3(1.0f,-0.15f,0.1f));//Rotate
  //   M = glm::mat4(1);//Translate
  //   M = glm::translate(M, glm::vec3(0.0f,0.0f,0.1f*i));//Scale
  //   M = glm::scale(M, glm::vec3(15.0f,15.0f,15.0f));//Scale
  //
  //   simpleShader.BindMatrices(&M,&V,&P);
  //
  //   ourModel.Draw(simpleShader);
  //   // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 21);
  //
  //   frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  //   distance=Loss(image1, frameImage, favg,cff);
  //   var_distance=last_distance-distance;
  //   last_distance=distance;
  //   cout << distance <<" "<<var_distance << endl;
  //
  //   /* unbind frameBuffer */
  //   glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // }



  // float distance=1.0;
  // std::vector< unsigned char > frameImage;
  // float last_distance=1.0;
  // float var_distance=1.0;
  // for(unsigned int i=0;((var_distance>MIN_ERROR)&&(i<MAX_ITER));i++)
  // {
  //   // //BEGIN_TEST MahalanobisDistance
  //   glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  //   /* Render here */
  //   // Clear the colorbuffer
  //   glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  //   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //
  //   simpleShader.Use();
  //
  //   //Calculate View Matrix
  //   // cam.position.z=cam.position.z-0.1;
  //   // updateCamera(&cam,&V);
  //   // simpleShader.BindMatrices(&M,&V,&P,&lightPos);
  //
  //   M = glm::rotate(M, glm::radians(73.8f), glm::vec3(1.0f,-0.15f,0.1f));//Rotate
  //   M = glm::mat4(1);//Translate
  //   M = glm::translate(M, glm::vec3(0.0f,0.0f,0.1f*i));//Scale
  //   M = glm::scale(M, glm::vec3(15.0f,15.0f,15.0f));//Scale
  //
  //   simpleShader.BindMatrices(&M,&V,&P);
  //
  //   ourModel.Draw(simpleShader);
  //   // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 21);
  //
  //   frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  //   distance=Loss(image1, frameImage, favg,cff);
  //   var_distance=last_distance-distance;
  //   last_distance=distance;
  //   cout << distance <<" "<<var_distance << endl;
  //
  //   /* unbind frameBuffer */
  //   glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // }

  // cam.position.z=cam.position.z+0.1;
  // updateCamera(&cam,&V);
  // distance=1.0;
  // last_distance=1.0;
  // var_distance=1.0;
  // for(unsigned int i=0;((var_distance>MIN_ERROR)&&(i<MAX_ITER));i++)
  // {
  //   // //BEGIN_TEST MahalanobisDistance
  //   glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  //   /* Render here */
  //   // Clear the colorbuffer
  //   glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  //   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //
  //   simpleShader.Use();
  //
  //   //Calculate View Matrix
  //   M = glm::mat4(1);//Translate
  //   M = glm::rotate(M, glm::radians(73.8f), glm::vec3(1.0f,(-0.1f-(0.001f*i)),0.1f));//Rotate
  //   M = glm::scale(M, glm::vec3(15.0f,15.0f,15.0f));//Scale
  //   // simpleShader.BindMatrices(&M,&V,&P,&lightPos);
  //   simpleShader.BindMatrices(&M,&V,&P);
  //
  //   ourModel.Draw(simpleShader);
  //   // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 21);
  //
  //   frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  //   distance=Loss(image1, frameImage, favg,cff);
  //   var_distance=last_distance-distance;
  //   last_distance=distance;
  //   cout << distance <<" "<<var_distance << endl;
  //
  //   /* unbind frameBuffer */
  //   glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // }

  // //Texture Projection
  // glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 21);
  // // frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  // std::vector< float > newUV = GetNewUV(WINDOW_WIDTH, WINDOW_HEIGHT);
  // // MapTex(ourModel,frameImage,newUV,(unsigned int)WINDOW_WIDTH,(unsigned int)WINDOW_HEIGHT);
  // MapTex(ourModel,imgProj,newUV,(unsigned int)WINDOW_WIDTH,(unsigned int)WINDOW_HEIGHT);
  // glBindFramebuffer(GL_FRAMEBUFFER, 0);






  glfwSetWindowShouldClose(window, 1);

  glfwTerminate();
  return 0;
}
