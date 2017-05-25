//Esqueleto do render de obj
#include <iostream>
#include <vector>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
//
#include "camera.h"
#include "createShader.h"
#include "model.h"
#include "SOIL.h"
#include "MyImage.h"
#include "lossfunc.h"
#include "pose.h"
#include "frameBuffer.h"
#include "Downhill.h"
#include "heuristicAprox.h"
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

//




void reshape(int w, int h, glm::mat4* P)
{
  glViewport(0,0,(GLsizei) w, (GLsizei) h);
  // float focalLenght=22.85;
  // float FOV= 2 * glm::arctan(w/(focalLenght * 2));
  // cout<<"\nFOV="<<FOV<<endl;
  // *P = glm::perspective(FOV, (GLfloat) w/ (GLfloat) h, 10.0f, 800.0f);
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

int main(int argc,char** argv)
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
  Mat imageGray;
  Mat imageColor;
  uint8_t *imgProj;
  //LossFunction
  double favg;
  double cff;

  //Initialize Model
  Model ourModel;

  // Initialize Shader
  // Shader PhongShader("shaders/PhongTex_VS.glsl","shaders/PhongTex_FS.glsl");
  Shader simpleShader("shaders/simple_VS.glsl","shaders/simple_FS.glsl");
  Shader textureShader("shaders/Tex_VS.glsl","shaders/Tex_FS.glsl");

  int opt;
  int p=0;
  int m=0;

  while ((opt = getopt(argc,argv,"p:m:d")) != EOF)
  {
    switch(opt)
    {
      case 'p':
      {
        p=1;
        cout << "value of p is"<< optarg <<endl ;
        imageGray = imread(optarg, CV_LOAD_IMAGE_GRAYSCALE);
        imageColor = imread(optarg, CV_LOAD_IMAGE_COLOR);
        toGray(imageColor,imageGray);//use average desaturate istead of lightness
        // imwrite("out/teste.jpg",imageGray);
        imgProj = imageColor.data;
        favg=Mean(imageGray);
        cff=MyCff(imageGray,favg);
        break;
      }
      case 'm':
      {
        m=1;
        cout << "value of m is"<< optarg <<endl ;
        ourModel.loadModel(optarg);
        break;
      }
      case '?':
      {
        cout<<"-p photopath -m modelpath"<<endl;
        return -1;
      }
      default:
      {
        cout<<endl; abort();
      }
    }
  }

  if(p==0)
  {
    //Default
    //Images
    imageGray = imread("../material/frogFu256.JPG", CV_LOAD_IMAGE_GRAYSCALE);
    imageColor = imread("../material/frogFu2048.JPG", CV_LOAD_IMAGE_COLOR);
    toGray(imageColor,imageGray);//use average desaturate istead of lightness
    // imwrite("out/teste.jpg",imageGray);
    imgProj = imageColor.data;
    favg=Mean(imageGray);
    cff=MyCff(imageGray,favg);
  }
  if(m==0)
  {
    //Default
    //Initialize Model
    ourModel.loadModel("../models/frogfacecam2.obj");
  }


  //Initialize Light only for phong
  // glm::vec3 lightPos=glm::vec3(-10.0,10.0,10.0);
  // glm::vec4 lightPos=glm::vec4(-10.0,10.0,100.0,1.0);

  //Create FrameBuffer
  GLuint myFrameBuffer;
  GenerateFBO(&myFrameBuffer, WINDOW_WIDTH, WINDOW_HEIGHT);

  M = glm::mat4(1);//Translate
  updateCamera(&cam,&V);



  ////TEST CUBE FACES FOR BEST INITIAL pose
  // float distance=1.0;
  std::vector< unsigned char > frameImage;
  float firstscale=40.0f;
  float firstx=0;
  float firsty=0;
  float w[9*10];
  float fw[10] = {2.0f,3.0f,4.0f, 5.0f,6.0f,7.0f ,8.0f,9.0f,10.0f,11.f};
  // float rotVec[16] = {
  //    1.0f, 0.0f, 0.0f, 0.0f,
  //    0.0f, 1.0f, 0.0f, 0.0f,
  //    0.0f, 0.0f, 1.0f, 0.0f,
  //    0.0f, 0.0f, 0.0f, 1.0f
  // };
  // // float rotVecNew[16] = {
  // //    0.0f, 0.0f, -1.0f, 0.0f,
  // //    0.0f, -1.0f, 0.0f, 0.0f,
  // //    -1.0f, 0.0f, 0.0f, 0.0f,
  // //    0.0f, 0.0f, 0.0f, 1.0f
  // // };
  // float tempVec[7];



  // glm::mat4 rotMat;
  // memcpy( glm::value_ptr( rotMat ), rotVec, sizeof( rotVec ) );

  glm::mat4 initialRot;
  //F
  initialRot[0][0]=-1.0f;
  initialRot[0][1]=0.0f;
  initialRot[0][2]=0.0f;
  initialRot[0][3]=0.0f;

  initialRot[1][0]=0.0f;
  initialRot[1][1]=-1.0f;
  initialRot[1][2]=0.0f;
  initialRot[1][3]=0.0f;

  initialRot[2][0]=0.0f;
  initialRot[2][1]=0.0f;
  initialRot[2][2]=1.0f;
  initialRot[2][3]=0.0f;

  initialRot[3][0]=0.0f;
  initialRot[3][1]=0.0f;
  initialRot[3][2]=0.0f;
  initialRot[3][3]=1.0f;
  //B
  // initialRot[0][0]=1.0f;
  // initialRot[0][1]=0.0f;
  // initialRot[0][2]=0.0f;
  // initialRot[0][3]=0.0f;
  //
  // initialRot[1][0]=0.0f;
  // initialRot[1][1]=-1.0f;
  // initialRot[1][2]=0.0f;
  // initialRot[1][3]=0.0f;
  //
  // initialRot[2][0]=0.0f;
  // initialRot[2][1]=0.0f;
  // initialRot[2][2]=-1.0f;
  // initialRot[2][3]=0.0f;
  //
  // initialRot[3][0]=0.0f;
  // initialRot[3][1]=0.0f;
  // initialRot[3][2]=0.0f;
  // initialRot[3][3]=1.0f;

  //Top
  // initialRot[0][0]=1.0f;
  // initialRot[0][1]=0.0f;
  // initialRot[0][2]=0.0f;
  // initialRot[0][3]=0.0f;
  //
  // initialRot[1][0]=0.0f;
  // initialRot[1][1]=0.0f;
  // initialRot[1][2]=1.0f;
  // initialRot[1][3]=0.0f;
  //
  // initialRot[2][0]=0.0f;
  // initialRot[2][1]=-1.0f;
  // initialRot[2][2]=0.0f;
  // initialRot[2][3]=0.0f;
  //
  // initialRot[3][0]=0.0f;
  // initialRot[3][1]=0.0f;
  // initialRot[3][2]=0.0f;
  // initialRot[3][3]=1.0f;

  //SideL
  // initialRot[0][0]=0.0f;
  // initialRot[0][1]=0.0f;
  // initialRot[0][2]=-1.0f;
  // initialRot[0][3]=0.0f;
  //
  // initialRot[1][0]=0.0f;
  // initialRot[1][1]=-1.0f;
  // initialRot[1][2]=0.0f;
  // initialRot[1][3]=0.0f;
  //
  // initialRot[2][0]=-1.0f;
  // initialRot[2][1]=0.0f;
  // initialRot[2][2]=0.0f;
  // initialRot[2][3]=0.0f;
  //
  // initialRot[3][0]=0.0f;
  // initialRot[3][1]=0.0f;
  // initialRot[3][2]=0.0f;
  // initialRot[3][3]=1.0f;

  // //SideR
  // initialRot[0][0]=0.0f;
  // initialRot[0][1]=0.0f;
  // initialRot[0][2]=1.0f;
  // initialRot[0][3]=0.0f;
  //
  // initialRot[1][0]=0.0f;
  // initialRot[1][1]=-1.0f;
  // initialRot[1][2]=0.0f;
  // initialRot[1][3]=0.0f;
  //
  // initialRot[2][0]=1.0f;
  // initialRot[2][1]=0.0f;
  // initialRot[2][2]=0.0f;
  // initialRot[2][3]=0.0f;
  //
  // initialRot[3][0]=0.0f;
  // initialRot[3][1]=0.0f;
  // initialRot[3][2]=0.0f;
  // initialRot[3][3]=1.0f;

  //Fa
  // initialRot[0][0]=-1;
  // initialRot[0][1]=0;
  // initialRot[0][2]=0;
  // initialRot[1][0]=0;
  // initialRot[1][1]=-0.707107;
  // initialRot[1][2]=0.707107;
  // initialRot[2][0]=0;
  // initialRot[2][1]=0.707107;
  // initialRot[2][2]=0.707107;

  // initialRot[0][0]=-0.999982;
  // initialRot[0][1]=-0.00602429;
  // initialRot[0][2]=-0.00537507;
  // initialRot[1][0]=-0.00298387;
  // initialRot[1][1]=-0.562812;
  // initialRot[1][2]=0.82658;
  // initialRot[2][0]=-0.00516819;
  // initialRot[2][1]=0.826563;
  // initialRot[2][2]=0.562784;
  // firstscale=32.8546;
  // firstx=0.43204;
  // firsty=-3.08552;


  // cubefaces(fw,
  //         w,
  //         myFrameBuffer,
  //         simpleShader,
  //         ourModel,
  //         M,
  //         V,
  //         P,
  //         favg,
  //         cff,
  //         imageGray,
  //         firstscale,
  //         initialRot,
  //         firstx,
  //         firsty
  //       );

  //R
  // initialRot[0][0]=0.0f;
  // initialRot[0][1]=0.0f;
  // initialRot[0][2]=1.0f;
  // initialRot[0][3]=0.0f;
  //
  // initialRot[1][0]=0.0f;
  // initialRot[1][1]=-1.0f;
  // initialRot[1][2]=0.0f;
  // initialRot[1][3]=0.0f;
  //
  // initialRot[2][0]=1.0f;
  // initialRot[2][1]=0.0f;
  // initialRot[2][2]=0.0f;
  // initialRot[2][3]=0.0f;
  //
  // initialRot[3][0]=0.0f;
  // initialRot[3][1]=0.0f;
  // initialRot[3][2]=0.0f;
  // initialRot[3][3]=1.0f;

  // glm::vec3 myZ=cross(glm::vec3(w[0],w[1],w[2]),glm::vec3(w[3],w[4],w[5]) );
  //
  // initialRot[0][0]=w[0];//x1
  // initialRot[0][1]=w[3];//y1
  // initialRot[0][2]=myZ[0];//z1
  //
  // initialRot[1][0]=w[1];//x2
  // initialRot[1][1]=w[4];//y2
  // initialRot[1][2]=myZ[1];//z2
  //
  // initialRot[2][0]=w[2];//x3
  // initialRot[2][1]=w[5];//y3
  // initialRot[2][2]=myZ[2];//z3




  //aprox
  cubeAprox(fw,
          w,
          myFrameBuffer,
          simpleShader,
          ourModel,
          M,
          V,
          P,
          favg,
          cff,
          imageGray,
          firstscale,
          firstx,
          firsty,
          initialRot
        );

  //scale
  scaleAprox(fw,
          w,
          myFrameBuffer,
          simpleShader,
          ourModel,
          M,
          V,
          P,
          favg,
          cff,
          imageGray,
          firstscale,
          firstx,
          firsty
        );

  // cout << "\n--Random--"<< endl;
  // for(int i=0;i<100;i++)
  // {
  //   // Bind FrameBuffer
  //   glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  //   /* Render here */
  //   // Clear the colorbuffer
  //   glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  //   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //
  //
  //   simpleShader.Use();
  //
  //   //Transformations (Translate*Rotate*Scale)
  //   M = glm::mat4(1);//Init
  //   // M = glm::translate(M, glm::vec3(0.0f,0.0f,0.0f));//Translate
  //
  //   glm::vec3 myZ=cross(glm::vec3(w[0],w[1],w[2]),glm::vec3(w[3],w[4],w[5]) );
  //
  //   rotVecNew[0]=w[0];//x1
  //   rotVecNew[1]=w[3];//y1
  //   rotVecNew[2]=myZ[0];//z1
  //
  //   rotVecNew[4]=w[1];//x2
  //   rotVecNew[5]=w[4];//y2
  //   rotVecNew[6]=myZ[1];//z2
  //
  //   rotVecNew[8]=w[2];//x3
  //   rotVecNew[9]=w[5];//y3
  //   rotVecNew[10]=myZ[2];//z3
  //
  //   memcpy( glm::value_ptr( rotMat ), rotVecNew, sizeof( rotVecNew ) );
  //   rotMat = glm::rotate(rotMat, (float)(rand()%10), glm::vec3( specialRand(), specialRand(), specialRand() ));//Rotate
  //   M = M * rotMat;
  //   int theta= specialRand()*10 ;
  //   M = glm::scale(M, glm::vec3(firstscale+theta,firstscale+theta,firstscale+theta));//Scale
  //
  //   simpleShader.BindMatrices(&M,&V,&P);
  //
  //   ourModel.Draw(simpleShader);
  //   SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, i);
  //   frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  //   distance=Loss(imageGray, frameImage, favg,cff);
  //
  //   cout << distance << endl;
  //
  //   const float *pface = (const float*)glm::value_ptr(rotMat);
  //   tempVec[0]=pface[0];
  //   tempVec[1]=pface[4];
  //   tempVec[2]=pface[8];
  //
  //   tempVec[3]=pface[1];
  //   tempVec[4]=pface[5];
  //   tempVec[5]=pface[9];
  //   tempVec[6]=firstscale+theta;
  //   insertionFancy(fw,distance,tempVec,w);
  //
  //   /* unbind frameBuffer */
  //   glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // }

  cout<< endl;
  for(int i=0;i<8;i++)
  {
    cout << fw[i]<< " ";
  }
  cout<< endl;
  cout << "w[6]="<< w[6] << endl;
  ////END TEST CUBE FACES
  downhill(fw,
          w,
          myFrameBuffer,
          simpleShader,
          ourModel,
          M,
          V,
          P,
          favg,
          cff,
          imageGray,
          firstscale
        );


  //Texture Projection
  glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 80);
  frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  std::vector< float > newUV = GetNewUV(WINDOW_WIDTH, WINDOW_HEIGHT);
  // MapTex(ourModel,imgProj,newUV,(unsigned int)WINDOW_WIDTH,(unsigned int)WINDOW_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Bind FrameBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
    /* Render here */
    // Clear the colorbuffer
    glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    textureShader.Use();

    //Transformations (Translate*Rotate*Scale)
    M = glm::mat4(1);//Init
    M = glm::translate(M, glm::vec3(w[7],w[8],0.0f));//Translate
    glm::vec3 myZ=cross(glm::vec3(w[0],w[1],w[2]),glm::vec3(w[3],w[4],w[5]) );
    cout << "{" << myZ[0] << "," << myZ[1] << "," << myZ[2] << "} " << endl << endl;


    initialRot[0][0]=w[0];//x1
    initialRot[1][0]=w[3];//y1
    initialRot[2][0]=myZ[0];//z1

    initialRot[0][1]=w[1];//x2
    initialRot[1][1]=w[4];//y2
    initialRot[2][1]=myZ[1];//z2

    initialRot[0][2]=w[2];//x3
    initialRot[1][2]=w[5];//y3
    initialRot[2][2]=myZ[2];//z3

    // memcpy( glm::value_ptr( rotMat ), rotVecNew, sizeof( rotVecNew ) );
    M = M * initialRot;
    M = glm::scale(M, glm::vec3(w[6],w[6],w[6]) );//Scale

    textureShader.BindMatrices(&M,&V,&P);

    ourModel.Draw(textureShader);
    SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 333);

    glfwSetWindowShouldClose(window, 1);

  glfwTerminate();
  return 0;
}
