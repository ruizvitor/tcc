//Esqueleto do render de obj
#include <iostream>
#include <vector>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
//
// #include "camera.h"
#include "createShader.h"
#include "model.h"
#include "SOIL.h"
#include "MyImage.h"
#include "lossfunc.h"
#include "pose.h"
#include "mask.h"
#include "frameBuffer.h"
#include "initialCalibration.h"
#include "render.h"
#include "heuristicAprox.h"
#include "newDownHill.h"
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

using namespace std;


void updateCamera(glm::mat4& V)
{
  V = glm::lookAt(
								glm::vec3(0,0,100), // Camera is at (0,0,100), in World Space
								glm::vec3(0,0,0), // and looks at the origin
								glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
						   );
}

int main(int argc,char** argv)
{
  //Some definitions
  glm::mat4 P,V,M; // Projection, View, Model Matrices
  glm::mat4 T,R,S; // Translation, Rotation, Scale
  float near;

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
  // window = glfwCreateWindow(WINDOW_WIDTH, height, "Scene", NULL, NULL);
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
  int f=0;
  int h=0;
  int c=0;
  int I=0;
  int height=0;
  int width=0;
  int tx=0;
  int ty=0;
  point original;
  // glm::mat4 initialRot;
  Mat mask;
  while ((opt = getopt(argc,argv,"p:m:f:h:c:i")) != EOF)
  {
    switch(opt)
    {
      case 'p':
      {
        p=1;
        cout << "value of p is "<< optarg <<endl ;
        // imageGray = imread(optarg, CV_LOAD_IMAGE_GRAYSCALE);
        imageColor = imread(optarg, CV_LOAD_IMAGE_COLOR);
        break;
      }
      case 'm':
      {
        m=1;
        cout << "value of m is "<< optarg <<endl ;
        ourModel.loadModel(optarg);
        break;
      }
      case 'f':
      {
        f=1;
        cout << "value of f is "<< optarg <<endl ;
        getBetterInitPose(near,T,R,S,tx,ty,optarg);
        break;
      }
      case 'h':
      {
        cout<<"-f posefilePath -p photopath -m modelpath  -h 0 or 1 for downhillsimplex"<<endl;
        h=1;
        // c=0;
        break;
      }
      case 'c':
      {
        cout<<"-f posefilePath -p photopath -m modelpath  -h 0 or 1 for downhillsimplex"<<endl;
        c=1;
        // h=0;
        break;
      }
      case 'i':
      {
        cout<<"-f posefilePath -p photopath -m modelpath  -h 0 or 1 for downhillsimplex"<<endl;
        I=1;
        // h=0;
        break;
      }
      default:
      {
        cout<<"-f posefilePath -p photopath -m modelpath  -h 0 or 1 for downhillsimplex"<<endl; abort();
        break;
      }
    }
  }

  if(f==0)
  {
    //Default
    getBetterInitPose(near,T,R,S,tx,ty,"initial/backNew.txt");
  }
  if(p==0)
  {
    //Default
    //Images
    // imageGray = imread(optarg, CV_LOAD_IMAGE_GRAYSCALE);
    cout<<"will load image"<<endl;
    imageColor = imread("../material/testes/back4x.JPG", CV_LOAD_IMAGE_COLOR);
  }

  Mat translation_matrix;
  translation_matrix = (Mat_<float>(2,3) << 1, 0, tx,
                                            0, 1, ty);
  height=imageColor.rows;
  width=imageColor.cols;
  imageGray = Mat(width,height,CV_8UC1);
  warpAffine(imageColor,imageColor, translation_matrix,
            Size(width,height));
  cvtColor(imageColor,imageGray,CV_BGR2GRAY);
  // toGray(imageColor,imageGray);//use average desaturate istead of lightness
  // imwrite("out/prev.png",imageGray);
  getBoundingBox(imageGray,original);
  imgProj = imageColor.data;
  mask=makeMask(imageGray);
  if(I==1)
  {
    invertGray(imageGray, mask);
    // imwrite("invertTest.png",imageGray);
  }
  favg=Mean(imageGray);
  cff=MyCff(imageGray,favg);
  if(m==0)
  {
    //Default
    //Initialize Model
    ourModel.loadModel("../models/testes/MyfrogLSCM45Centered.obj");
    cout<<"Loaded Default model"<<endl;
  }


  //Init viewport
  glViewport(0,0,(GLsizei) width, (GLsizei) height);

  //Initialize Light only for phong
  // glm::vec3 lightPos=glm::vec3(-10.0,10.0,10.0);
  // glm::vec4 lightPos=glm::vec4(-10.0,10.0,100.0,1.0);

  //Create FrameBuffer
  GLuint myFrameBuffer;
  GenerateFBO(&myFrameBuffer, width, height);
  std::vector< unsigned char > frameImage;
  M=T*R*S;//Translate
  //Calculate Projection Matrix
  reshape(width, height, P, near);
  //Calculate View Matrix
  updateCamera(V);
  double distance=1.0;
  render(
          myFrameBuffer,
          simpleShader,
          ourModel,
          M,
          V,
          P,
          width,
          height,
          frameImage
        );
  // ApplyMask(mask,frameImage);
  distance=Loss(imageGray, frameImage, favg,cff);
  cout << distance << endl;

  ofstream disOUTPUT;
  disOUTPUT.open ("disOUTPUT.txt", ios::out | ios::trunc );
  disOUTPUT << "pre = "<< distance << endl;

  SaveFrameN(frameImage,width,height,111,imageColor );

  // boxfocal(original,
  //       myFrameBuffer,
  //       simpleShader,
  //       ourModel,
  //       M,
  //       V,
  //       P,
  //       T,
  //       R,
  //       S,
  //       width,
  //       height,
  //       frameImage
  //     );
  // SaveFrameN(frameImage,width,height,111 );

  // aprox
  // cubeAprox(fw,
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
  //         firstx,
  //         firsty,
  //         initialRot
  //       );

  float ORIGINALSCALEx=S[0][0]*P[0][0];
  float ORIGINALSCALEy=S[1][1]*P[1][1];

  if(c==1){
    cout << "will enter paramcomplete" << endl;

    ////TEST CUBE FACES FOR BEST INITIAL pose

    // float scaleX=1.0f;
    // float scaleY=1.0f;
    // float scaleZ=1.0f;

    float w[10*11];
    float fw[11];


    for(int j=0;j<11;j++)
    {
      fw[j]=2.0f+j;
    }


    for(int i=0;i<10;i++)
    {
      for(int j=0;j<11;j++)
      {
        w[(i*11)+j]=2.0f+j;
      }
    }

    paramsAproxComplete(fw,
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
                T,
                R,
                S,
                width,
                height,
                frameImage,
                near,
                tx,
                ty
              );
    cout<< endl;

    for(int i=0;i<11;i++)
    {
      cout << "fw["<<i<<"]="<<fw[i]<< " , ";
      for(int j=0;j<10;j++)
      {
        cout << w[(i*10)+j]<< " ";
      }
      cout<<endl;
    }
    cout<<endl;

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
            width,
            height,
            T,
            R,
            S,
            10,
            11,
            ORIGINALSCALEx,
            ORIGINALSCALEy
          );
    for(int i=0;i<11;i++)
    {
      cout << "fw["<<i<<"]="<<fw[i]<< " , ";
      for(int j=0;j<10;j++)
      {
        cout << w[(i*10)+j]<< " ";
      }
      cout<<endl;
    }
    cout<<endl;
    float tempVec[N_DIMENSIONSSUM];
    tempVec[0]=w[0];
    tempVec[1]=w[1];
    tempVec[2]=w[2];

    tempVec[3]=w[3];
    tempVec[4]=w[4];
    tempVec[5]=w[5];
    tempVec[6]=w[6];
    calculateZ(tempVec,R);

    reshape(width,height,P,w[6]);

    int tx=round(w[8]*(width/10));
    int ty=round(w[9]*(height/10));
    // int tx=w[8];
    // int ty=w[9];
    cout<<"tx="<<tx<<endl;
    cout<<"ty="<<ty<<endl;
    Mat translation_matrix = (Mat_<float>(2,3) << 1, 0, tx,
                                                  0, 1, ty);
    warpAffine(imageColor,imageColor, translation_matrix,
              Size(width,height));

    warpAffine(imageGray,imageGray, translation_matrix,
              Size(width,height),INTER_LINEAR, BORDER_CONSTANT);
    float SCALEFACTOR=w[7];
    float SCALEFACTORx=(ORIGINALSCALEx/(P[0][0]))*SCALEFACTOR;
    float SCALEFACTORy=(ORIGINALSCALEy/(P[1][1]))*SCALEFACTOR;
    S = glm::mat4(1);
    S = glm::scale(S, glm::vec3(SCALEFACTORx,SCALEFACTORy,(SCALEFACTORx+SCALEFACTORy)/2 ));//Scale


  }

  if(h==1){
    ////TEST CUBE FACES FOR BEST INITIAL pose
    // float scaleX=1.0f;
    // float scaleY=1.0f;
    // float scaleZ=1.0f;
    float w[N_DIMENSIONS*N_DIMENSIONSSUM];
    float fw[N_DIMENSIONSSUM];


    for(int j=0;j<N_DIMENSIONSSUM;j++)
    {
      fw[j]=2.0f+j;
    }


    for(int i=0;i<N_DIMENSIONS;i++)
    {
      for(int j=0;j<N_DIMENSIONSSUM;j++)
      {
        w[(i*N_DIMENSIONSSUM)+j]=2.0f+j;
      }
    }

    float tempVec[N_DIMENSIONSSUM];
    //paramsAprox
    paramsAprox(fw,
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
                T,
                R,
                S,
                width,
                height,
                frameImage,
                near,
                mask,
                original
              );



    cout<< endl;
    for(int i=0;i<N_DIMENSIONSSUM;i++)
    {
      cout << fw[i]<< " ";
    }
    cout<< endl << "params of best so far" <<endl;
    for(int i=0;i<N_DIMENSIONS;i++)
    {
      cout << w[i]<< " ";
    }
    cout<< endl;
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
            width,
            height,
            T,
            R,
            S,
            N_DIMENSIONS,
            N_DIMENSIONSSUM,
            ORIGINALSCALEx,
            ORIGINALSCALEy
          );
    for(int i=0;i<N_DIMENSIONSSUM;i++)
    {
      cout << "fw["<<i<<"]="<<fw[i]<< " , ";
      for(int j=0;j<N_DIMENSIONS;j++)
      {
        cout << w[(i*N_DIMENSIONS)+j]<< " ";
      }
      cout<<endl;
    }
    cout<<endl;
    tempVec[0]=w[0];
    tempVec[1]=w[1];
    tempVec[2]=w[2];

    tempVec[3]=w[3];
    tempVec[4]=w[4];
    tempVec[5]=w[5];
    tempVec[6]=w[6];
    calculateZ(tempVec,R);


    reshape(width,height,P,w[6]);

    cout<<"----out------"<<endl;
    cout<<w[6]<<endl<<endl;
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        cout<<T[i][j]<<" ";
      }
      cout<<endl;
    }
    cout<<endl;
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        cout<<R[i][j]<<" ";
      }
      cout<<endl;
    }
    cout<<endl;
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        cout<<S[i][j]<<" ";
      }
      cout<<endl;
    }
    cout<<endl;
    cout<<tx<<" "<<ty<<endl;
    cout<<"----out------"<<endl;

    ofstream myfile;
    myfile.open ("pose.txt", ios::out | ios::trunc );
    myfile<<w[6]<<endl<<endl;
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        myfile<<T[i][j]<<" ";
      }
      myfile<<endl;
    }
    myfile<<endl;
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        myfile<<R[i][j]<<" ";
      }
      myfile<<endl;
    }
    myfile<<endl;
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        myfile<<S[i][j]<<" ";
      }
      myfile<<endl;
    }
    myfile<<endl;
    myfile<<tx<<" "<<ty<<endl;
    myfile.close();
  }



  M=T*R*S;
  render(
          myFrameBuffer,
          simpleShader,
          ourModel,
          M,
          V,
          P,
          width,
          height,
          frameImage
        );
  distance=Loss(imageGray, frameImage, favg,cff);
  cout << distance << endl;
  SaveFrameN(frameImage,width,height,888,imageColor );
  disOUTPUT << "pos = "<< distance << endl;

  //Texture Projection
  glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer);
  frameImage = GetFrame(width, height);
  // ApplyMask(mask,(std::vector< unsigned char >&)imgProj);
  std::vector< float > newUV = GetNewUV(width, height);
  MapTex(ourModel,imgProj,newUV,(unsigned int)width,(unsigned int)height);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glfwSetWindowShouldClose(window, 1);

  disOUTPUT.close();
  glfwTerminate();
  return 0;
}
