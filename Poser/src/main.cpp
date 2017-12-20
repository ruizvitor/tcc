/*
  Augmented Reality Application
  Developed by Daniel Vitor Ruiz

  Uses:
  OpenGL     3D Render Pipeline
  GLFW       Window-event handler
  GLEW       The OpenGL Extension Wrangler Library
  ASSIMP     3D models importer
  OpenCV     Image importer-processor and computer vision lib
*/
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <opencv2/opencv.hpp>

#include "macros.hpp"
#include "createShader.hpp"
#include "model.hpp"
#include "render.hpp"
#include "initialCalibration.hpp"
#include "heuristic.hpp"
#include "lossfunction.hpp"

int face1=0;
int face2=0;
int face3=0;
int face4=0;
int face5=0;
int face6=0;

int activatelaplacian=0;

int rotate_x_increase=0;
int rotate_x_decrease=0;
int rotate_y_increase=0;
int rotate_y_decrease=0;
int rotate_z_increase=0;
int rotate_z_decrease=0;

int scale_increase=0;
int scale_decrease=0;
int x_increase=0;
int x_decrease=0;

int y_increase=0;
int y_decrease=0;

// int z_increase=0;
// int z_decrease=0;
int reset=0;
int print=0;
int near_increase=0;
int near_decrease=0;

using namespace std;
using namespace cv;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
    {
      if(activatelaplacian==0)
        activatelaplacian=1;
      else
        activatelaplacian=0;
    }
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
      face1=1;
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
      face2=1;
    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
      face3=1;
    }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
    {
      face4=1;
    }
    if (key == GLFW_KEY_5 && action == GLFW_PRESS)
    {
      face5=1;
    }
    if (key == GLFW_KEY_6 && action == GLFW_PRESS)
    {
      face6=1;
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    //RESET
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
      reset=1;
    }
    //PRINT
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
      print=1;
    }
    //scale
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
      scale_increase=1;
    }
    if (key == GLFW_KEY_W && action == GLFW_RELEASE)
    {
      scale_increase=0;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
      scale_decrease=1;
    }
    if (key == GLFW_KEY_S && action == GLFW_RELEASE)
    {
      scale_decrease=0;
    }
    //Xmove
    if (key == GLFW_KEY_J && action == GLFW_PRESS)
    {
      x_increase=1;
    }
    if (key == GLFW_KEY_J && action == GLFW_RELEASE)
    {
      x_increase=0;
    }
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
      x_decrease=1;
    }
    if (key == GLFW_KEY_L && action == GLFW_RELEASE)
    {
      x_decrease=0;
    }
    //Ymove
    if (key == GLFW_KEY_I && action == GLFW_PRESS)
    {
      y_increase=1;
    }
    if (key == GLFW_KEY_I && action == GLFW_RELEASE)
    {
      y_increase=0;
    }
    if (key == GLFW_KEY_K && action == GLFW_PRESS)
    {
      y_decrease=1;
    }
    if (key == GLFW_KEY_K && action == GLFW_RELEASE)
    {
      y_decrease=0;
    }

    // //Zmove
    // if (key == GLFW_KEY_T && action == GLFW_PRESS)
    // {
    //   z_increase=1;
    // }
    // if (key == GLFW_KEY_T && action == GLFW_RELEASE)
    // {
    //   z_increase=0;
    // }
    // if (key == GLFW_KEY_G && action == GLFW_PRESS)
    // {
    //   z_decrease=1;
    // }
    // if (key == GLFW_KEY_G && action == GLFW_RELEASE)
    // {
    //   z_decrease=0;
    // }

    //near
    if (key == GLFW_KEY_N && action == GLFW_PRESS)
    {
      near_increase=1;
    }
    // if (key == GLFW_KEY_N && action == GLFW_RELEASE)
    // {
    //   near_increase=0;
    // }
    if (key == GLFW_KEY_M && action == GLFW_PRESS)
    {
      near_decrease=1;
    }


    //rotateX
    if (key == GLFW_KEY_T && action == GLFW_PRESS)
    {
      rotate_x_increase=1;
    }
    if (key == GLFW_KEY_T && action == GLFW_RELEASE)
    {
      rotate_x_increase=0;
    }
    if (key == GLFW_KEY_G && action == GLFW_PRESS)
    {
      rotate_x_decrease=1;
    }
    if (key == GLFW_KEY_G && action == GLFW_RELEASE)
    {
      rotate_x_decrease=0;
    }

    //rotateY
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
      rotate_y_increase=1;
    }
    if (key == GLFW_KEY_F && action == GLFW_RELEASE)
    {
      rotate_y_increase=0;
    }
    if (key == GLFW_KEY_H && action == GLFW_PRESS)
    {
      rotate_y_decrease=1;
    }
    if (key == GLFW_KEY_H && action == GLFW_RELEASE)
    {
      rotate_y_decrease=0;
    }

    //rotateZ
    if (key == GLFW_KEY_V && action == GLFW_PRESS)
    {
      rotate_z_increase=1;
    }
    if (key == GLFW_KEY_V && action == GLFW_RELEASE)
    {
      rotate_z_increase=0;
    }
    if (key == GLFW_KEY_B && action == GLFW_PRESS)
    {
      rotate_z_decrease=1;
    }
    if (key == GLFW_KEY_B && action == GLFW_RELEASE)
    {
      rotate_z_decrease=0;
    }
}

int main(int argc,char** argv)
{
  // Mat TheInputImage=imread("../models/front4x.JPG",CV_LOAD_IMAGE_GRAYSCALE);
  // Mat TheInputImage=imread("/home/dvr13/tg-1/material/testes/back8x.JPG",CV_LOAD_IMAGE_COLOR);
  // Mat TheInputImage=imread("/home/dvr13/tg-1/material/testes/front24x.JPG",CV_LOAD_IMAGE_COLOR);
  Mat TheInputImage;
  point original;
  if(argc > 1){
    TheInputImage=imread(argv[1],CV_LOAD_IMAGE_COLOR);
  }
  else{
    TheInputImage=imread("/home/dvr13/tg-1/material/testes/front4x.JPG",CV_LOAD_IMAGE_COLOR);
  }
  getBoundingBoxColor((std::vector< unsigned char >&)TheInputImage.data,original,TheInputImage.rows,TheInputImage.cols);
  rectangle(TheInputImage,Point(original.xmin,original.ymin),Point(original.x,original.y),255 );

  // Mat TheInputImage=imread("/home/dvr13/tg-1/material/testes/front24x.JPG",CV_LOAD_IMAGE_COLOR);
  int height=TheInputImage.size().height;
  int width=TheInputImage.size().width;
  // vector< unsigned char > image( width * height * 3 );
  // unsigned char* tempImage = TheInputImage.data;

  cvtColor(TheInputImage,TheInputImage,CV_BGR2RGB);//Fix Opencv BGR to RGB
  cout << "will flip."<< endl;
  Mat flip_image;
  flip(TheInputImage,flip_image,0);//vertical



  // flip_image=TheInputImage;

  //Some definitions
  glm::mat4 P,V,M; // Projection, View, Model Matrices
  GLFWwindow* window;

  // int height=WINDOW_HEIGHT;
  // int width=WINDOW_WIDTH;
  cout << "width=" << width << " height=" << height << endl;

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
  window = glfwCreateWindow(width,height, "ARapp", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  //set key handler
  glfwSetKeyCallback(window, key_callback);

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  // Initialize GLEW to setup the OpenGL Function pointers
  glewExperimental = GL_TRUE;
  glewInit();

  // Setup some OpenGL options
  glEnable(GL_DEPTH_TEST);

  // Initialize Shader
  Shader textureShader("shaders/Tex_VS.glsl","shaders/Tex_FS.glsl");
  Shader simpleShader("shaders/simple_VS.glsl","shaders/simple_FS.glsl");
  Shader screenShader("shaders/screen_VS.glsl","shaders/screen_FS.glsl");
  screenShader.Use();
  screenShader.setInt("screenTexture", 0);

  //Initialize Model
  Model ourModel;
  //Initialize Model
  cout << "model will load."<< endl;
  // ourModel.loadModel("../models/boySimpleCenter.obj");
  if(argc > 2){
    ourModel.loadModel(argv[2]);
  }
  else
  {
    ourModel.loadModel("/home/dvr13/tg-1/models/testes/MyfrogLSCM45Centered.obj");
  }
  // ourModel.loadModel("/home/dvr13/ARVISION/models/cube.obj");
  cout << "model loaded successfully."<< endl;

  //Create FrameBuffer
  FrameBuffer myFrameBuffer(width, height);

  P = glm::mat4(1);
  V = glm::mat4(1);
  M = glm::mat4(1);
  //Init viewport
  glViewport(0, 0, width, height);
  // reshape(width,height,P,INITIAL_NEAR);
  updateCamera(V);
  ScreenQuad squad;


  // Size TheGlWindowSize=Size(width,height);

  glm::mat4 initialRot;
  glm::mat4 T;
  glm::mat4 R;
  glm::mat4 S;

  float nearFACTOR=INITIAL_NEAR;
  float ORIGINALSCALEx;
  float ORIGINALSCALEy;

  float SCALEFACTOR=1.0f;
  float SCALEFACTORx;
  float SCALEFACTORy;
  float xFACTOR=0.0;
  float yFACTOR=0.0;
  // float zFACTOR=0.0;
  float xRotate=0.0;
  float yRotate=0.0;
  float zRotate=0.0;
  int specialreset=0;
  float filenear=0;

  std::vector< unsigned char > frameImage;
  std::vector< unsigned char > temp(width*height*3);
  point rect;
  Mat warped=flip_image.clone();
  Mat translation_matrix;

  int originalX;
  int originalY;
  if(argc > 3)
  {
    specialreset=1;
    getBetterInitPose(nearFACTOR,T,initialRot,S,originalX,originalY,argv[3]);
    R=initialRot;
    filenear=nearFACTOR;
    reshape(width,height,P,INITIAL_NEAR);
    ORIGINALSCALEx=S[0][0]*P[0][0];
    ORIGINALSCALEy=S[1][1]*P[1][1];
    originalY=-originalY;
    translation_matrix = (Mat_<float>(2,3) << 1, 0, originalX,
                                              0, 1, originalY);

    warpAffine(flip_image,warped, translation_matrix,
              Size(width,height));
    cout<< "-------warp------"<<endl;
    cout<< originalX << " " << -(originalY)<<endl;

    SCALEFACTORx=(ORIGINALSCALEx/(P[0][0]))*SCALEFACTOR;
    SCALEFACTORy=(ORIGINALSCALEy/(P[1][1]))*SCALEFACTOR;
  }
  else
  {
    //Default
    initialRot[0][0]=1.0f;
    initialRot[0][1]=0.0f;
    initialRot[0][2]=0.0f;
    initialRot[0][3]=0.0f;

    initialRot[1][0]=0.0f;
    // initialRot[1][1]=1.0f;//original -1
    initialRot[1][1]=-1.0f;//original -1
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

    T = glm::mat4(1);
    R = initialRot;
    S = glm::mat4(1);
    reshape(width,height,P,INITIAL_NEAR);
    ORIGINALSCALEx=INITIAL_SCALE*P[0][0];
    ORIGINALSCALEy=INITIAL_SCALE*P[1][1];

    SCALEFACTORx=(ORIGINALSCALEx/(P[0][0]))*SCALEFACTOR;
    SCALEFACTORy=(ORIGINALSCALEy/(P[1][1]))*SCALEFACTOR;
    S = glm::scale(S, glm::vec3(SCALEFACTORx,SCALEFACTORy,(SCALEFACTORx+SCALEFACTORy)/2 ));//Scale


    boxfocal(original,
            rect,
            myFrameBuffer.FBO,
            simpleShader,
            ourModel,
            M,
            V,
            P,
            T,
            R,
            S,
            width,
            height,
            frameImage
          );

    originalX=rect.centerx-original.centerx;
    originalY=rect.centery-original.centery;
    translation_matrix = (Mat_<float>(2,3) << 1, 0, originalX,
                                              0, 1, originalY);

    warpAffine(flip_image,warped, translation_matrix,
              Size(width,height));
    cout<< "-------warp------"<<endl;
    cout<< originalX << " " << -(originalY)<<endl;
    ORIGINALSCALEx=S[0][0]*P[0][0];
    ORIGINALSCALEy=S[1][1]*P[1][1];
    SCALEFACTORx=(ORIGINALSCALEx/(P[0][0]))*SCALEFACTOR;
    SCALEFACTORy=(ORIGINALSCALEy/(P[1][1]))*SCALEFACTOR;
  }

  // rectangle(flip_image,Point(rect.xmin,rect.ymin),Point(rect.x,rect.y),255 );


  glm::mat4 MVP=(T*R*S)*V*P;
  cout<< "-------V------"<<endl;

  for(int i=0;i<4;i++)
  {
    for(int j=0;j<4;j++)
    {
      cout<< V[i][j] <<" ";
    }
    cout<<endl;
  }
  cout<< "-------pvm------"<<endl;

  for(int i=0;i<4;i++)
  {
    for(int j=0;j<4;j++)
    {
      cout<< MVP[i][j] <<" ";
    }
    cout<<endl;
  }

  Mat src=Mat(height, width, CV_8UC3);

  Mat clon=Mat::zeros(height, width, CV_8UC3);



  while (!glfwWindowShouldClose(window))
  {
    // TheVideoCapturer.grab();
    // TheVideoCapturer.retrieve( TheInputImage);
    // tempImage = TheInputImage.data;
    // cvtColor(TheInputImage,TheInputImage,CV_BGR2RGB);//Fix Opencv BGR to RGB
    // flipImg(tempImage, image, width, height);//OPENGL INVERT Y

    if(near_increase)
    {
      near_increase=0;
      nearFACTOR+=0.1;
      reshape(width,height,P,nearFACTOR);
      SCALEFACTORx=(ORIGINALSCALEx/(P[0][0]))*SCALEFACTOR;
      SCALEFACTORy=(ORIGINALSCALEy/(P[1][1]))*SCALEFACTOR;
    }
    if(near_decrease)
    {
      near_decrease=0;
      nearFACTOR-=0.1;
      reshape(width,height,P,nearFACTOR);
      SCALEFACTORx=(ORIGINALSCALEx/(P[0][0]))*SCALEFACTOR;
      SCALEFACTORy=(ORIGINALSCALEy/(P[1][1]))*SCALEFACTOR;
      // SCALEFACTOR+=5.0;
    }
    reshape(width,height,P,nearFACTOR);


    if(scale_increase)
    {
      SCALEFACTOR+=0.01;
      reshape(width,height,P,nearFACTOR);
      SCALEFACTORx=(ORIGINALSCALEx/(P[0][0]))*SCALEFACTOR;
      SCALEFACTORy=(ORIGINALSCALEy/(P[1][1]))*SCALEFACTOR;
    }
    if(scale_decrease)
    {
      SCALEFACTOR-=0.01;
      reshape(width,height,P,nearFACTOR);
      SCALEFACTORx=(ORIGINALSCALEx/(P[0][0]))*SCALEFACTOR;
      SCALEFACTORy=(ORIGINALSCALEy/(P[1][1]))*SCALEFACTOR;
    }


    if(x_increase)
    {
      // xFACTOR-=0.1;
      xFACTOR-=1.0;
    }
    if(x_decrease)
    {
      // xFACTOR+=0.1;
      xFACTOR+=1.0;
    }

    if(y_increase)
    {
      // yFACTOR+=0.1;
      yFACTOR+=1.0;
    }
    if(y_decrease)
    {
      // yFACTOR-=0.1;
      yFACTOR-=1.0;
    }

    // if(z_increase)
    // {
    //   zFACTOR-=0.1;
    // }
    // if(z_decrease)
    // {
    //   zFACTOR+=0.1;
    // }

    if(rotate_x_increase)
    {
      xRotate+=0.1;
    }
    if(rotate_x_decrease)
    {
      xRotate-=0.1;
    }

    if(rotate_y_increase)
    {
      yRotate+=0.1;
    }
    if(rotate_y_decrease)
    {
      yRotate-=0.1;
    }
    if(rotate_z_increase)
    {
      zRotate+=0.1;
    }
    if(rotate_z_decrease)
    {
      zRotate-=0.1;
    }

    if(face1){
      face1=0;
      initialRot[0][0]=1.0f;
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

      // point rect;
      // boxfocal(original,
      //         rect,
      //         myFrameBuffer.FBO,
      //         simpleShader,
      //         ourModel,
      //         M,
      //         V,
      //         P,
      //         T,
      //         R,
      //         S,
      //         width,
      //         height,
      //         frameImage
      //       );
      // ORIGINALSCALEx=S[0][0]*P[0][0];
      // ORIGINALSCALEy=S[1][1]*P[1][1];
      // SCALEFACTORx=(ORIGINALSCALEx/(P[0][0]))*SCALEFACTOR;
      // SCALEFACTORy=(ORIGINALSCALEy/(P[1][1]))*SCALEFACTOR;
    }

    if(face2){
      face2=0;
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
      initialRot[2][2]=-1.0f;
      initialRot[2][3]=0.0f;

      initialRot[3][0]=0.0f;
      initialRot[3][1]=0.0f;
      initialRot[3][2]=0.0f;
      initialRot[3][3]=1.0f;

      // point rect;
      // boxfocal(original,
      //         rect,
      //         myFrameBuffer.FBO,
      //         simpleShader,
      //         ourModel,
      //         M,
      //         V,
      //         P,
      //         T,
      //         R,
      //         S,
      //         width,
      //         height,
      //         frameImage
      //       );
      // ORIGINALSCALEx=S[0][0]*P[0][0];
      // ORIGINALSCALEy=S[1][1]*P[1][1];
      // SCALEFACTORx=(ORIGINALSCALEx/(P[0][0]))*SCALEFACTOR;
      // SCALEFACTORy=(ORIGINALSCALEy/(P[1][1]))*SCALEFACTOR;
    }

    if(face3){
      face3=0;
      initialRot[0][0]=0.0f;
      initialRot[0][1]=0.0f;
      initialRot[0][2]=-1.0f;
      initialRot[0][3]=0.0f;

      initialRot[1][0]=0.0f;
      initialRot[1][1]=-1.0f;
      initialRot[1][2]=0.0f;
      initialRot[1][3]=0.0f;

      initialRot[2][0]=1.0f;
      initialRot[2][1]=0.0f;
      initialRot[2][2]=0.0f;
      initialRot[2][3]=0.0f;

      initialRot[3][0]=0.0f;
      initialRot[3][1]=0.0f;
      initialRot[3][2]=0.0f;
      initialRot[3][3]=1.0f;
    }

    if(face4){
      face4=0;
      initialRot[0][0]=0.0f;
      initialRot[0][1]=0.0f;
      initialRot[0][2]=1.0f;
      initialRot[0][3]=0.0f;

      initialRot[1][0]=0.0f;
      initialRot[1][1]=-1.0f;
      initialRot[1][2]=0.0f;
      initialRot[1][3]=0.0f;

      initialRot[2][0]=-1.0f;
      initialRot[2][1]=0.0f;
      initialRot[2][2]=0.0f;
      initialRot[2][3]=0.0f;

      initialRot[3][0]=0.0f;
      initialRot[3][1]=0.0f;
      initialRot[3][2]=0.0f;
      initialRot[3][3]=1.0f;
    }
    if(face5){
      face5=0;
      initialRot[0][0]=1.0f;
      initialRot[0][1]=0.0f;
      initialRot[0][2]=0.0f;
      initialRot[0][3]=0.0f;

      initialRot[1][0]=0.0f;
      initialRot[1][1]=0.0f;
      initialRot[1][2]=-1.0f;
      initialRot[1][3]=0.0f;

      initialRot[2][0]=0.0f;
      initialRot[2][1]=-1.0f;
      initialRot[2][2]=0.0f;
      initialRot[2][3]=0.0f;

      initialRot[3][0]=0.0f;
      initialRot[3][1]=0.0f;
      initialRot[3][2]=0.0f;
      initialRot[3][3]=1.0f;
    }
    if(face6){
      face6=0;
      initialRot[0][0]=1.0f;
      initialRot[0][1]=0.0f;
      initialRot[0][2]=0.0f;
      initialRot[0][3]=0.0f;

      initialRot[1][0]=0.0f;
      initialRot[1][1]=0.0f;
      initialRot[1][2]=1.0f;
      initialRot[1][3]=0.0f;

      initialRot[2][0]=0.0f;
      initialRot[2][1]=1.0f;
      initialRot[2][2]=0.0f;
      initialRot[2][3]=0.0f;

      initialRot[3][0]=0.0f;
      initialRot[3][1]=0.0f;
      initialRot[3][2]=0.0f;
      initialRot[3][3]=1.0f;
    }

    if(reset)
    {
      reset=0;
      if(specialreset==1){
        nearFACTOR=filenear;
      }
      else
      {
        nearFACTOR=INITIAL_NEAR;
      }
      reshape(width,height,P,nearFACTOR);
      SCALEFACTOR=1.0f;
      SCALEFACTORx=(ORIGINALSCALEx/(P[0][0]))*SCALEFACTOR;
      SCALEFACTORy=(ORIGINALSCALEy/(P[1][1]))*SCALEFACTOR;
      xFACTOR=0.0f;
      yFACTOR=0.0;
      // zFACTOR=0.0f;
      xRotate=0.0f;
      yRotate=0.0f;
      zRotate=0.0f;
    }

    // P=glm::translate(P,glm::vec3(xFACTOR+2.0f/width, yFACTOR+2.0/height ,0.0f) );
    // Transformations (Translate*Rotate*Scale)
    // glViewport(xFACTOR, yFACTOR, width+xFACTOR, height+yFACTOR);
    T = glm::mat4(1);
    // T[3][0]=xFACTOR;
    // T[3][1]=yFACTOR;
    // T[3][2]=zFACTOR;
    R = initialRot;
    R = glm::rotate(R, glm::radians(xRotate), glm::vec3(1.0f,0.0f,0.0f));//Rotate
    R = glm::rotate(R, glm::radians(yRotate), glm::vec3(0.0f,1.0f,0.0f));//Rotate
    R = glm::rotate(R, glm::radians(zRotate), glm::vec3(0.0f,0.0f,1.0f));//Rotate
    S = glm::mat4(1);
    // float currentFrame = fmod(glfwGetTime(),360)*10;
    S = glm::scale(S, glm::vec3(SCALEFACTORx,SCALEFACTORy,(SCALEFACTORx+SCALEFACTORy)/2 ));//Scale

    // boxfocal(original,
    //         myFrameBuffer.FBO,
    //         simpleShader,
    //         ourModel,
    //         M,
    //         V,
    //         P,
    //         T,
    //         R,
    //         S,
    //         width,
    //         height,
    //         frameImage
    //       );
    M=T*R*S;

    if(print)
    {
      print=0;
      cout<<endl<<"near="<<nearFACTOR <<endl;
      cout<< "-------T------"<<endl;
      for(int i=0;i<4;i++)
      {
        for(int j=0;j<4;j++)
        {
          cout<< T[i][j] <<" ";
        }
        cout<<endl;
      }

      cout<< "-------R------"<<endl;
      for(int i=0;i<4;i++)
      {
        for(int j=0;j<4;j++)
        {
          cout<< R[i][j] <<" ";
        }
        cout<<endl;
      }

      cout<< "-------S------"<<endl;
      for(int i=0;i<4;i++)
      {
        for(int j=0;j<4;j++)
        {
          cout<< S[i][j] <<" ";
        }
        cout<<endl;
      }
      cout<< "-------warp------"<<endl;
      cout<< originalX+xFACTOR << " " << -(originalY+yFACTOR)<<endl;

      flip(src,src,0);//vertical
      cvtColor(src,src,CV_BGR2RGB);
      imwrite("test.png",src);

      // favg=Mean(warped);
      // cff=MyCff(warped,favg);
      // std::vector< unsigned char > frameImage(clon.begin<unsigned char>(), clon.end<unsigned char>());
      // distance=Loss(warped, frameImage, favg,cff);
      // cout << "distance=" << distance << endl;
      // cout<< "-------P------"<<endl;
      // for (int i = 0; i < 4; i++)
      // {
      //   for (int j = 0; j < 4; j++)
      //   {
      //     cout<<P[i][j]<<" ";
      //   }
      //   cout<<endl;
      // }
      // cout<< "-------MVP------"<<endl;
      // MVP=(T*R*S)*V*P;
      // for(int i=0;i<4;i++)
      // {
      //   for(int j=0;j<4;j++)
      //   {
      //     cout<< MVP[i][j] <<" ";
      //   }
      //   cout<<endl;
      // }

    }

    translation_matrix = (Mat_<float>(2,3) << 1, 0, originalX+xFACTOR,
                                              0, 1, originalY+yFACTOR);
    warpAffine(flip_image,warped, translation_matrix,
              Size(width,height));

    add(warped,clon,warped);
    // render(
    //         myFrameBuffer,
    //         simpleShader,
    //         textureShader,
    //         screenShader,
    //         ourModel,
    //         M,
    //         V,
    //         P,
    //         width,
    //         height,
    //         warped,
    //         squad
    //       );
    render(
            myFrameBuffer,
            textureShader,
            simpleShader,
            screenShader,
            ourModel,
            M,
            V,
            P,
            width,
            height,
            warped,
            squad
          );
    // glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer.FBO);
    temp=GetFrame(width,height);

    getBoundingBoxColor(temp,rect,height,width);

    src = Mat(height, width, CV_8UC3);
    src.data=temp.data();
    clon=src.clone();
    // flip(src,src,0);//vertical
    // imwrite("test.png",src);

    if(activatelaplacian==1)
    {
      mylaplacian(clon,width,height);
    }
    rectangle(clon,Point(rect.xmin,rect.ymin),Point(rect.x,rect.y),255 );

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }



  glfwTerminate();
  return 0;
}
