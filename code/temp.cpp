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
#include "Downhill.h"
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
//




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
  // Mat imageGray = imread("../material/FrogSintetico.jpg", CV_LOAD_IMAGE_GRAYSCALE);
  // Mat imageGray = imread("../material/cj_b2.JPG", CV_LOAD_IMAGE_GRAYSCALE);
  Mat imageGray = imread("../material/cj_l3.JPG", CV_LOAD_IMAGE_GRAYSCALE);
  // Myimage imgProj("../material/ff0.JPG");
  // Mat imageColor = imread("../material/cj_b2.JPG", CV_LOAD_IMAGE_COLOR);
  Mat imageColor = imread("../material/cj_l3.JPG", CV_LOAD_IMAGE_COLOR);
  // Mat imageColor = imread("../material/FrogSintetico.jpg", CV_LOAD_IMAGE_COLOR);
  uint8_t *imgProj = imageColor.data;
  // Mat imageGray = imread("../material/frontClean.JPG", CV_LOAD_IMAGE_GRAYSCALE);
  double favg=Mean(imageGray);
  double cff=MyCff(imageGray,favg);

  // Initialize Shader
  // Shader PhongShader("shaders/PhongTex_VS.glsl","shaders/PhongTex_FS.glsl");
  Shader simpleShader("shaders/simple_VS.glsl","shaders/simple_FS.glsl");
  //Initialize Model
  Model ourModel("../models/frogfacecam.obj");
  // Model ourModel("../models/joelNew.obj");


  //Initialize Light only for phong
  // glm::vec3 lightPos=glm::vec3(-10.0,10.0,10.0);
  // glm::vec4 lightPos=glm::vec4(-10.0,10.0,100.0,1.0);

  //Create FrameBuffer
  GLuint myFrameBuffer;
  GenerateFBO(&myFrameBuffer, WINDOW_WIDTH, WINDOW_HEIGHT);

  M = glm::mat4(1);//Translate
  updateCamera(&cam,&V);



  ////TEST CUBE FACES FOR BEST INITIAL pose
  float distance=1.0;
  std::vector< unsigned char > frameImage;
  float firstscale=40.0f;
  glm::vec3 myscale(firstscale,firstscale,firstscale);
  float w[7*8];
  float fw[8] = {2.0f,3.0f,4.0f, 5.0f,6.0f,7.0f ,8.0f,9.0f};
  float rotVec[16] = {
     1.0f, 0.0f, 0.0f, 0.0f,
     0.0f, 1.0f, 0.0f, 0.0f,
     0.0f, 0.0f, 1.0f, 0.0f,
     0.0f, 0.0f, 0.0f, 1.0f
  };
  float rotVecNew[16] = {
     0.0f, 0.0f, -1.0f, 0.0f,
     0.0f, -1.0f, 0.0f, 0.0f,
     -1.0f, 0.0f, 0.0f, 0.0f,
     0.0f, 0.0f, 0.0f, 1.0f
  };
  float tempVec[7];

  // float temp[8] = {68.0f,67.0f,4.0f, 65.0f,66.0f,77.0f ,80.0f,9.0f};
  // float tempp[8];
  // BottomUpMergeSort(temp,tempp,8);
  // cout << "mergesort: " << temp[0] << " " << temp[1] << " " << temp[2] << " " << temp[3] << " "<< temp[4] << " "<< temp[5] << " "<< temp[6] << " "<< temp[7]<< endl;

  glm::mat4 rotMat;
  memcpy( glm::value_ptr( rotMat ), rotVec, sizeof( rotVec ) );



  //CUBEFACES
  // for(int j=0;j<4;j++)
  // {
  //   for(int k=0;k<4;k++)
  //   {
  //     //Bind FrameBuffer
  //     glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  //     /* Render here */
  //     // Clear the colorbuffer
  //     glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //
  //
  //     simpleShader.Use();
  //
  //     //Transformations (Translate*Rotate*Scale)
  //     M = glm::mat4(1);//Init
  //     // M = glm::translate(M, glm::vec3(0.0f,0.0f,0.0f));//Translate
  //     memcpy( glm::value_ptr( rotMat ), rotVec, sizeof( rotVec ) );
  //     rotMat = glm::rotate(rotMat, glm::radians(90.0f*k), glm::vec3(0.0f,0.0f,1.0f));//Rotate
  //     rotMat = glm::rotate(rotMat, glm::radians(90.0f*j), glm::vec3(1.0f,0.0f,0.0f));//Rotate
  //     M = M * rotMat;
  //     M = glm::scale(M, glm::vec3(firstscale,firstscale,firstscale));//Scale
  //
  //     simpleShader.BindMatrices(&M,&V,&P);
  //
  //     ourModel.Draw(simpleShader);
  //     // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, (j*10)+(k));
  //     frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  //     distance=Loss(imageGray, frameImage, favg,cff);
  //
  //     cout << distance << endl;
  //
  //     const float *pface = (const float*)glm::value_ptr(rotMat);
  //     tempVec[0]=pface[0];
  //     tempVec[1]=pface[4];
  //     tempVec[2]=pface[8];
  //
  //     tempVec[3]=pface[1];
  //     tempVec[4]=pface[5];
  //     tempVec[5]=pface[9];
  //     tempVec[6]=firstscale;
  //     insertionFancy(fw,distance,tempVec,w);
  //
  //     /* unbind frameBuffer */
  //     glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //   }
  // }
  //
  //
  //
  // for(int j=1;j<4;j+=2)
  // {
  //   for(int k=0;k<4;k++)
  //   {
  //     //Bind FrameBuffer
  //     glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  //     /* Render here */
  //     // Clear the colorbuffer
  //     glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
  //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //
  //
  //     simpleShader.Use();
  //
  //     //Transformations (Translate*Rotate*Scale)
  //     M = glm::mat4(1);//Init
  //     // M = glm::translate(M, glm::vec3(0.0f,0.0f,-12.5f));//Translate
  //     memcpy( glm::value_ptr( rotMat ), rotVec, sizeof( rotVec ) );
  //     rotMat = glm::rotate(rotMat, glm::radians(90.0f*k), glm::vec3(0.0f,0.0f,1.0f));//Rotate
  //     rotMat = glm::rotate(rotMat, glm::radians(90.0f*j), glm::vec3(0.0f,1.0f,0.0f));//Rotate
  //     M = M * rotMat;
  //     M = glm::scale(M, glm::vec3(firstscale,firstscale,firstscale));//Scale
  //
  //     simpleShader.BindMatrices(&M,&V,&P);
  //
  //     ourModel.Draw(simpleShader);
  //     // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, (100)+(j*10)+(k));
  //     frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  //     distance=Loss(imageGray, frameImage, favg,cff);
  //
  //     cout << distance << endl;
  //     const float *pface = (const float*)glm::value_ptr(rotMat);
  //     tempVec[0]=pface[0];
  //     tempVec[1]=pface[4];
  //     tempVec[2]=pface[8];
  //
  //     tempVec[3]=pface[1];
  //     tempVec[4]=pface[5];
  //     tempVec[5]=pface[9];
  //     tempVec[6]=firstscale;
  //     insertionFancy(fw,distance,tempVec,w);
  //
  //     /* unbind frameBuffer */
  //     glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //   }
  // }

  for(int i=0;i<5;i++)
  {
    for(int j=0;j<5;j++)
    {
      for(int k=0;k<5;k++)
      {
        //Bind FrameBuffer
        glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
        /* Render here */
        // Clear the colorbuffer
        glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        simpleShader.Use();

        //Transformations (Translate*Rotate*Scale)
        M = glm::mat4(1);//Init
        // M = glm::translate(M, glm::vec3(0.0f,0.0f,0.0f));//Translate
        memcpy( glm::value_ptr( rotMat ), rotVecNew, sizeof( rotVecNew ) );
        rotMat = glm::rotate(rotMat, glm::radians(-20.0f*k), glm::vec3(0.0f,0.0f,1.0f));//Rotate
        rotMat = glm::rotate(rotMat, glm::radians(-10.0f*i), glm::vec3(0.0f,1.0f,0.0f));//Rotate
        rotMat = glm::rotate(rotMat, glm::radians(-10.0f*j), glm::vec3(1.0f,0.0f,0.0f));//Rotate
        M = M * rotMat;
        M = glm::scale(M, glm::vec3(firstscale,firstscale,firstscale));//Scale

        simpleShader.BindMatrices(&M,&V,&P);

        ourModel.Draw(simpleShader);
        // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, (i*100)+(j*10)+(k));
        // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, (i*100));
        frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
        distance=Loss(imageGray, frameImage, favg,cff);

        cout << distance << endl;

        const float *pface = (const float*)glm::value_ptr(rotMat);
        tempVec[0]=pface[0];
        tempVec[1]=pface[4];
        tempVec[2]=pface[8];

        tempVec[3]=pface[1];
        tempVec[4]=pface[5];
        tempVec[5]=pface[9];
        tempVec[6]=firstscale;
        insertionFancy(fw,distance,tempVec,w);

        /* unbind frameBuffer */
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
      }
    }
  }

  for(int i=0;i<5;i++)
  {
    for(int j=0;j<5;j++)
    {
      for(int k=0;k<5;k++)
      {
        //Bind FrameBuffer
        glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
        /* Render here */
        // Clear the colorbuffer
        glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        simpleShader.Use();

        //Transformations (Translate*Rotate*Scale)
        M = glm::mat4(1);//Init
        // M = glm::translate(M, glm::vec3(0.0f,0.0f,0.0f));//Translate
        memcpy( glm::value_ptr( rotMat ), rotVecNew, sizeof( rotVecNew ) );
        rotMat = glm::rotate(rotMat, glm::radians(20.0f*k), glm::vec3(0.0f,0.0f,1.0f));//Rotate
        rotMat = glm::rotate(rotMat, glm::radians(10.0f*i), glm::vec3(0.0f,1.0f,0.0f));//Rotate
        rotMat = glm::rotate(rotMat, glm::radians(10.0f*j), glm::vec3(1.0f,0.0f,0.0f));//Rotate
        M = M * rotMat;
        M = glm::scale(M, glm::vec3(firstscale,firstscale,firstscale));//Scale

        simpleShader.BindMatrices(&M,&V,&P);

        ourModel.Draw(simpleShader);
        // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, (i*100)+(j*10)+(k));
        // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, (i*100));
        frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
        distance=Loss(imageGray, frameImage, favg,cff);

        cout << distance << endl;

        const float *pface = (const float*)glm::value_ptr(rotMat);
        tempVec[0]=pface[0];
        tempVec[1]=pface[4];
        tempVec[2]=pface[8];

        tempVec[3]=pface[1];
        tempVec[4]=pface[5];
        tempVec[5]=pface[9];
        tempVec[6]=firstscale;
        insertionFancy(fw,distance,tempVec,w);

        /* unbind frameBuffer */
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
      }
    }
  }

  for(int k=0;k<5;k++)
  {
    //Bind FrameBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
    /* Render here */
    // Clear the colorbuffer
    glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    simpleShader.Use();

    //Transformations (Translate*Rotate*Scale)
    M = glm::mat4(1);//Init
    // M = glm::translate(M, glm::vec3(0.0f,0.0f,0.0f));//Translate

    glm::vec3 myZ=cross(glm::vec3(w[0],w[1],w[2]),glm::vec3(w[3],w[4],w[5]) );

    rotVecNew[0]=w[0];//x1
    rotVecNew[1]=w[3];//y1
    rotVecNew[2]=myZ[0];//z1

    rotVecNew[4]=w[1];//x2
    rotVecNew[5]=w[4];//y2
    rotVecNew[6]=myZ[1];//z2

    rotVecNew[8]=w[2];//x3
    rotVecNew[9]=w[5];//y3
    rotVecNew[10]=myZ[2];//z3
    memcpy( glm::value_ptr( rotMat ), rotVecNew, sizeof( rotVecNew ) );
    // rotMat = glm::rotate(rotMat, glm::radians(20.0f*k), glm::vec3(0.0f,0.0f,1.0f));//Rotate
    // rotMat = glm::rotate(rotMat, glm::radians(10.0f*i), glm::vec3(0.0f,1.0f,0.0f));//Rotate
    // rotMat = glm::rotate(rotMat, glm::radians(10.0f*j), glm::vec3(1.0f,0.0f,0.0f));//Rotate
    M = M * rotMat;
    M = glm::scale(M, glm::vec3(firstscale-k,firstscale-k,firstscale-k));//Scale

    simpleShader.BindMatrices(&M,&V,&P);

    ourModel.Draw(simpleShader);
    // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, (i*100)+(j*10)+(k));
    // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, (i*100));
    frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
    distance=Loss(imageGray, frameImage, favg,cff);

    cout << distance << endl;

    const float *pface = (const float*)glm::value_ptr(rotMat);
    tempVec[0]=pface[0];
    tempVec[1]=pface[4];
    tempVec[2]=pface[8];

    tempVec[3]=pface[1];
    tempVec[4]=pface[5];
    tempVec[5]=pface[9];
    tempVec[6]=firstscale-k;
    insertionFancy(fw,distance,tempVec,w);

    /* unbind frameBuffer */
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  for(int k=0;k<5;k++)
  {
    //Bind FrameBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
    /* Render here */
    // Clear the colorbuffer
    glClearColor(0.0,0.0,0.0,0);   // set the clear color to black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    simpleShader.Use();

    //Transformations (Translate*Rotate*Scale)
    M = glm::mat4(1);//Init
    // M = glm::translate(M, glm::vec3(0.0f,0.0f,0.0f));//Translate

    glm::vec3 myZ=cross(glm::vec3(w[0],w[1],w[2]),glm::vec3(w[3],w[4],w[5]) );

    rotVecNew[0]=w[0];//x1
    rotVecNew[1]=w[3];//y1
    rotVecNew[2]=myZ[0];//z1

    rotVecNew[4]=w[1];//x2
    rotVecNew[5]=w[4];//y2
    rotVecNew[6]=myZ[1];//z2

    rotVecNew[8]=w[2];//x3
    rotVecNew[9]=w[5];//y3
    rotVecNew[10]=myZ[2];//z3
    memcpy( glm::value_ptr( rotMat ), rotVecNew, sizeof( rotVecNew ) );
    // rotMat = glm::rotate(rotMat, glm::radians(20.0f*k), glm::vec3(0.0f,0.0f,1.0f));//Rotate
    // rotMat = glm::rotate(rotMat, glm::radians(10.0f*i), glm::vec3(0.0f,1.0f,0.0f));//Rotate
    // rotMat = glm::rotate(rotMat, glm::radians(10.0f*j), glm::vec3(1.0f,0.0f,0.0f));//Rotate
    M = M * rotMat;
    M = glm::scale(M, glm::vec3(firstscale+k,firstscale+k,firstscale+k));//Scale

    simpleShader.BindMatrices(&M,&V,&P);

    ourModel.Draw(simpleShader);
    // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, (i*100)+(j*10)+(k));
    // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, (i*100));
    frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
    distance=Loss(imageGray, frameImage, favg,cff);

    cout << distance << endl;

    const float *pface = (const float*)glm::value_ptr(rotMat);
    tempVec[0]=pface[0];
    tempVec[1]=pface[4];
    tempVec[2]=pface[8];

    tempVec[3]=pface[1];
    tempVec[4]=pface[5];
    tempVec[5]=pface[9];
    tempVec[6]=firstscale+k;
    insertionFancy(fw,distance,tempVec,w);

    /* unbind frameBuffer */
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  cout << "\n--Random--"<< endl;
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
          imageGray
        );


  //Texture Projection
  glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  // SaveFrame(WINDOW_WIDTH, WINDOW_HEIGHT, 80);
  frameImage = GetFrame(WINDOW_WIDTH, WINDOW_HEIGHT);
  std::vector< float > newUV = GetNewUV(WINDOW_WIDTH, WINDOW_HEIGHT);
  // MapTex(ourModel,frameImage,newUV,(unsigned int)WINDOW_WIDTH,(unsigned int)WINDOW_HEIGHT);
  MapTex(ourModel,imgProj,newUV,(unsigned int)WINDOW_WIDTH,(unsigned int)WINDOW_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glfwSetWindowShouldClose(window, 1);

  glfwTerminate();
  return 0;
}
