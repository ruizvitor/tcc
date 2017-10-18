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

int rotate_x_increase=0;
int rotate_x_decrease=0;
int rotate_y_increase=0;
int rotate_y_decrease=0;

int scale_increase=0;
int scale_decrease=0;
int x_increase=0;
int x_decrease=0;

int y_increase=0;
int y_decrease=0;

int z_increase=0;
int z_decrease=0;
int reset=0;
int print=0;
int near_increase=0;
int near_decrease=0;

using namespace std;
using namespace cv;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
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
  // rectangle(TheInputImage,Point(original.xmin,original.ymin),Point(original.x,original.y),255 );

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
  ourModel.loadModel("/home/dvr13/tg-1/models/testes/MyfrogLSCM45Centered.obj");
  // ourModel.loadModel("/home/dvr13/ARVISION/models/cube.obj");
  cout << "model loaded successfully."<< endl;

  //Create FrameBuffer
  FrameBuffer myFrameBuffer(width, height);

  P = glm::mat4(1);
  V = glm::mat4(1);
  M = glm::mat4(1);
  //Init viewport
  glViewport(0, 0, width, height);
  reshape(width,height,P,INITIAL_NEAR);
  updateCamera(V);
  ScreenQuad squad;


  // Size TheGlWindowSize=Size(width,height);



  glm::mat4 initialRot;

  //Default
  // initialRot[0][0]=-1.0f;
  // initialRot[0][1]=0.0f;
  // initialRot[0][2]=0.0f;
  // initialRot[0][3]=0.0f;
  //
  // initialRot[1][0]=0.0f;
  // initialRot[1][1]=1.0f;//original -1
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

  initialRot[0][0]=1.0f;
  initialRot[0][1]=0.0f;
  initialRot[0][2]=0.0f;
  initialRot[0][3]=0.0f;

  initialRot[1][0]=0.0f;
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

  float SCALEFACTOR=1.0f;
  float SCALEFACTORx;
  float SCALEFACTORy;
  float xFACTOR=0.0;
  float yFACTOR=0.0;
  float zFACTOR=0.0;
  float xRotate=0.0;
  float yRotate=0.0;
  float nearFACTOR=INITIAL_NEAR;
  std::vector< unsigned char > frameImage;

  double favg;
  double cff;
  double distance=1.0f;
  std::vector< unsigned char > temp(width*height*3);

  glm::mat4 T;
  glm::mat4 R;
  glm::mat4 S;
  T = glm::mat4(1);
  R = initialRot;
  S = glm::mat4(1);
  float ORIGINALSCALEx=INITIAL_SCALE*P[0][0];
  float ORIGINALSCALEy=INITIAL_SCALE*P[1][1];
  SCALEFACTORx=(ORIGINALSCALEx/(P[0][0]))*SCALEFACTOR;
  SCALEFACTORy=(ORIGINALSCALEy/(P[1][1]))*SCALEFACTOR;
  S = glm::scale(S, glm::vec3(SCALEFACTORx,SCALEFACTORy,(SCALEFACTORx+SCALEFACTORy)/2 ));//Scale


  point rect;
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

  Mat warped=flip_image.clone();
  Mat translation_matrix;
  translation_matrix = (Mat_<float>(2,3) << 1, 0, rect.centerx-original.centerx,
                                            0, 1, rect.centery-original.centery);
  // translation_matrix = (Mat_<float>(2,3) << 1, 0, 0,
  //                                           0, 1, rect.centery-original.centery);
  warpAffine(flip_image,warped, translation_matrix,
            Size(width,height));
  cout<< "-------warp------"<<endl;
  cout<< rect.centerx-original.centerx<< " " << -(rect.centery-original.centery)<<endl;

  // rectangle(flip_image,Point(rect.xmin,rect.ymin),Point(rect.x,rect.y),255 );

  ORIGINALSCALEx=S[0][0]*P[0][0];
  ORIGINALSCALEy=S[1][1]*P[1][1];
  SCALEFACTORx=(ORIGINALSCALEx/(P[0][0]))*SCALEFACTOR;
  SCALEFACTORy=(ORIGINALSCALEy/(P[1][1]))*SCALEFACTOR;
  glm::mat4 MVP=(T*R*S)*V*P;
  cout<< "-------pvm------"<<endl;

  for(int i=0;i<4;i++)
  {
    for(int j=0;j<4;j++)
    {
      cout<< MVP[i][j] <<" ";
    }
    cout<<endl;
  }

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

    if(z_increase)
    {
      zFACTOR-=0.1;
    }
    if(z_decrease)
    {
      zFACTOR+=0.1;
    }

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

    if(reset)
    {
      reset=0;
      nearFACTOR=INITIAL_NEAR;
      reshape(width,height,P,nearFACTOR);
      SCALEFACTOR=1.0f;
      SCALEFACTORx=(ORIGINALSCALEx/(P[0][0]))*SCALEFACTOR;
      SCALEFACTORy=(ORIGINALSCALEy/(P[1][1]))*SCALEFACTOR;
      xFACTOR=0.0f;
      yFACTOR=0.0;
      zFACTOR=0.0f;
      xRotate=0.0f;
      yRotate=0.0f;
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
      cout<< rect.centerx-original.centerx+xFACTOR<< " " << -(rect.centery-original.centery+yFACTOR)<<endl;

      favg=Mean(warped);
      cff=MyCff(warped,favg);
      temp=GetFrame(width,height);
      distance=Loss(warped, temp, favg,cff);
      cout << "distance=" << distance << endl;
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

    translation_matrix = (Mat_<float>(2,3) << 1, 0, rect.centerx-original.centerx+xFACTOR,
                                              0, 1, rect.centery-original.centery+yFACTOR);
    warpAffine(flip_image,warped, translation_matrix,
              Size(width,height));

    add(warped,clon,warped);
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
    Mat src = Mat(height, width, CV_8UC3);
    src.data=temp.data();
    clon=src.clone();
    rectangle(clon,Point(rect.xmin,rect.ymin),Point(rect.x,rect.y),255 );

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }



  glfwTerminate();
  return 0;
}