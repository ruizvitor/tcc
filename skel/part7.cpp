/// This skeleton only contains a lit sphere. We recommend to use your stuff from the
/// the previous assignmnet as skeleton.

// Put the names of all group members here
// Daniel Vitor Ruiz - Matrikelnummer: 21983193 - Benutzerkennung: yx17upyj
// Joao Victor Tozatti Risso - Matrikelnummer: 21983403 - Benutzerkennung: ih94yxad

#include <iostream>
#include "camera.h"
#include "helper.h"
#include "offLoader.h"


#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>




#include <pthread.h>
void* simpleFunc(void*) { return NULL; }
void forcePThreadLink() { pthread_t t1; pthread_create(&t1, NULL, &simpleFunc, NULL); }

bool wireframe_mode = false;

using namespace std;


GLuint earthTex,moonTex,saturnTex,backgroundTex;


float t  =0;  // the time parameter (incremented in the idle-function)
float speed = 0.1;  // rotation speed of the light source in degree/frame

int currentX,currentY; // global variables to store the current mouse position


//We need to keep track of matrices ourselves

/**
* @brief P,V,M:
* your matrices for setting the scene, no matrix stack anymore
*/
glm::mat4 P,V,M;

/**
* @brief The ShaderUniforms struct:
* every shader has its own uniforms,
* binding of uniforms is done in bindUniforms()
*
*/
struct ShaderUniforms
{
  GLuint Shader;
  GLint location_MVP;
  GLint location_MV;
  GLint location_NormalMatrix ;
  GLint location_Time;
  GLint location_LightSourceViewSpace;
  GLint location_Color;


  GLint location_Texture;

  void bindUniforms(glm::mat4& M, glm::mat4& V, glm::mat4& P, glm::vec4& LightSource, glm::vec4& Color,  GLuint TexID, float  t)
  {
    location_Time					= glGetUniformLocation(Shader, "Time");
    location_MVP					= glGetUniformLocation(Shader, "MVP");
    location_MV						= glGetUniformLocation(Shader, "MV");
    location_NormalMatrix			= glGetUniformLocation(Shader, "NormalMatrix");
    location_LightSourceViewSpace	= glGetUniformLocation(Shader, "LightSource");
    location_Color					= glGetUniformLocation(Shader, "Color");

    location_Texture				= glGetUniformLocation(Shader, "Texture");




    glm::mat4 MV			= V*M;
    glm::mat4 MVP			= P*MV;
    glm::mat3 NormalMatrix	= glm::transpose(glm::inverse(glm::mat3(MV)));


    glUniformMatrix4fv(location_MVP, 1, false, glm::value_ptr(MVP));
    glUniformMatrix4fv(location_MV, 1, false, glm::value_ptr(MV));
    glUniformMatrix3fv(location_NormalMatrix, 1, false, glm::value_ptr(NormalMatrix));
    glUniform4fv(location_LightSourceViewSpace, 1, glm::value_ptr(LightSource));
    glUniform4fv(location_Color, 1, glm::value_ptr(Color));

    glBindTexture(GL_TEXTURE_2D,TexID);

    glBindAttribLocation(Shader,0,"mypos");
    glBindAttribLocation(Shader,1,"mynormal");
    glBindAttribLocation(Shader,2,"mytexture");

    glUniform1f(location_Time, 10*t);
  }

};

/**
* @brief The ShaderUniforms struct:
* every shader has its own uniforms,
* binding of uniforms is done in bindUniforms()
*
*/
ShaderUniforms SunShader, TexturePhongShader; // the shaders

cameraSystem cam;   // the camera
/**
* @brief OffObject
*  Object for loading/parsing the shuttle file
*
*/
OffObject *objA;    // the shuttle


/**
* @brief The GeometryData struct:
* store the VertexArrayObject and number of vertices and indices
*/
struct GeometryData
{
  GLuint vao;
  unsigned int numVertices;
  unsigned int numIndices;
};

GeometryData geometryShuttle, geometryCube, geometrySphere, geometryRings ;

/**
* @brief The Vertex struct:
* store vertices with according normals
* and texture coordinates
*/
struct Vertex {

  Vertex(glm::vec3 p, glm::vec3 n )
  {
    position[0] = p.x;
    position[1] = p.y;
    position[2] = p.z;
    position[3] = 1;

    normal[0] = n.x;
    normal[1] = n.y;
    normal[2] = n.z;
    normal[3] = 1;

  };
  Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 t)
  {
    position[0] = p.x;
    position[1] = p.y;
    position[2] = p.z;
    position[3] = 1;

    normal[0] = n.x;
    normal[1] = n.y;
    normal[2] = n.z;
    normal[3] = 1;

    texcoord[0] = t.x;
    texcoord[1] = t.y;
  };

  GLfloat position[4];
  GLfloat normal[4];
  GLfloat texcoord[2];
};

void bindVertexArrayObjects(GeometryData& geometry, const std::vector<Vertex> &vertexdata, const std::vector<unsigned short> &indices)
{

  //*bind to GL
  glGenVertexArrays(1, &(geometry.vao));
  glBindVertexArray(geometry.vao);


  geometry.numVertices = vertexdata.size();
  geometry.numIndices = indices.size();
  std::cout << "Nr of vertices: " << geometry.numVertices << std::endl;
  std::cout << "Nr of indices: " << geometry.numIndices << std::endl;

  // Create and bind a BO for vertex data
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferData(GL_ARRAY_BUFFER, geometry.numVertices * sizeof(Vertex), vertexdata.data(), GL_STATIC_DRAW);


  // set up vertex attributes
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));

  // Create and bind a BO for index data
  GLuint ibo;
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  // copy data into the buffer object
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, geometry.numIndices * sizeof(unsigned short), indices.data(), GL_STATIC_DRAW);


  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void createRings()
{
  glm::vec2 t;
  glm::vec3 n(0,1,0);
  glm::vec3 p ;

  unsigned short index = 0;
  std::vector<Vertex> vertexdata ;
  std::vector<unsigned short> indices;



  float r= 1;
  for (int i=0;i<100;i++)
  {
    float arg = M_PI*(float)i/50.0;
    p =  glm::vec3(r*cos(arg),0,r*sin(arg));
    vertexdata.push_back(Vertex (p,n,t));
    indices.push_back(index++);
  }

  bindVertexArrayObjects(geometryRings,vertexdata,indices);

}


inline void uvMapping( float & u, float & v, const glm::vec3 & point)
{
  glm::vec3 d = glm::normalize(point);
  u = 0.5 + (atan2( d.z, d.x ) / (2.0 * M_PI));
  v = 0.5 - (asin( d.y ) / M_PI);
}

/// TODO
void createSphere()
{
  float r=1;
  int slices = 25;
  int stacks = 50;

  float dTheta = 2.0*M_PI/(float)stacks;
  float dPhi = M_PI/(float)slices;

  std::vector<Vertex> vertexdata ;
  std::vector<unsigned short> indices;

  glm::vec2 t;
  glm::vec3 n;
  glm::vec3 p;

  float u, v; // coordinates for UV-Mapping

  // ***********************************************************************************
  // TODO: modify your drawing of the sphere
  // take care not to create duplicate vertices
  // having an equal amount of vertices and indices defeats the purpose of index buffers

  // because we want to have a single buffer, we will use TRIANGLES, e.g. each quad consists of 2 triangles

  // we suggest
  // 1. create top most vertex
  // 2a. create first ring/slice: add 1 Vertex per stack
  // 2b. create 1 triangle per stack !NO VERTEX DUPLICATES!, close the ring with the first vertex you created for the stack
  /// -> you should be able to see the hat of the sphere (maybe use wire frame mode to contol)

  // 3.  create middle part
  // 3a. create the remaing rings/slices: add 1 Vertex per stack
  // 3b. create 2 triangles for every new vertex you add

  // 4. create bottmom most vertex
  // 4a. no additional vertices necessary
  // 4b. create 1.triangle per stack using the existing vertices of the lastly created slice/ring


  // Note: Vertex positions/normals/texcoords are identical to your existing code
  // wireframe rendering can be helpful

  // ***********************************************************************************

  //glm::vec2 t(0.5,1.0);
  //n=glm::vec3(0,1,0);
  //p=glm::vec3(0,r,0);
  //vertexdata.push_back(Vertex (p,n,t));

  // Geometry
  uvMapping( u, v, glm::vec3(0, r, 0) );
  t= glm::vec2( u, v );
  n=glm::vec3( 0, 1, 0 );
  p=glm::vec3(0,r,0);
  vertexdata.push_back(Vertex (p,n,t));

  for (int i = 0; i<stacks; i++) {
    glm::vec3 vertex = glm::vec3( r*sin(dPhi)*cos(i*dTheta), r*cos(dPhi), r*sin(dPhi)*sin(i*dTheta) );
    uvMapping( u, v, vertex );
    t=glm::vec2( u, v );
    n=glm::vec3( sin(dPhi)*cos(i*dTheta), cos(dPhi), sin(dPhi)*sin(i*dTheta) );
    p=glm::vec3( vertex.x, vertex.y, vertex.z );
    vertexdata.push_back(Vertex (p,n,t));
  }

  // Middle Part
  for (int j=1; j<slices; j++) {
    for (int i = 0; i<stacks; i++) {
      glm::vec3 vertex = glm::vec3( r*sin(j*dPhi)*cos(i*dTheta), r*cos(j*dPhi), r*sin(j*dPhi)*sin(i*dTheta) );
      uvMapping( u, v, vertex );
      t=glm::vec2( u, v );
      n=glm::vec3( sin(j*dPhi)*cos(i*dTheta), cos(j*dPhi), sin(j*dPhi)*sin(i*dTheta) );
      p=glm::vec3( vertex.x, vertex.y, vertex.z );
      vertexdata.push_back(Vertex (p,n,t));
    }
  }

  // South Pole:
  for (int i = 0; i<stacks; i++) {
    glm::vec3 vertex = glm::vec3( r*sin((slices)*dPhi)*cos(i*dTheta), r*cos((slices)*dPhi), r*sin((slices)*dPhi)*sin(i*dTheta) );
    uvMapping( u, v, vertex );
    t=glm::vec2( u, v );
    n=glm::vec3( sin((slices)*dPhi)*cos(i*dTheta), cos((slices)*dPhi), sin((slices)*dPhi)*sin(i*dTheta) );
    p=glm::vec3( vertex.x, vertex.y, vertex.z );
    vertexdata.push_back(Vertex (p,n,t));
  }

  uvMapping( u, v, glm::vec3(0, -r, 0) );
  t=glm::vec2( u, v );
  n=glm::vec3(0,-1,0);
  p=glm::vec3(0,-r,0);
  vertexdata.push_back(Vertex (p,n,t));

  //Topology
  // North Pole
  for(int i=1;i<=stacks;i++){
    indices.push_back(0);
    indices.push_back(i);
    indices.push_back(i+1);
  }
  indices.push_back(0);
  indices.push_back(stacks+1);
  indices.push_back(1);

  // Middle Part
  for(int j=0; j<slices; j++) {
    for(int i=1;i<stacks;i++){
      indices.push_back((j*stacks)+i);
      indices.push_back(((j+1)*stacks)+i);
      indices.push_back((j*stacks)+i+1);

      indices.push_back(((j+1)*stacks)+i);
      indices.push_back(((j+1)*stacks)+i+1);
      indices.push_back((j*stacks)+i+1);
    }

    indices.push_back(((j+1)*stacks));
    indices.push_back(((j+2)*stacks));
    indices.push_back((j*stacks)+1);

    indices.push_back((j+2)*stacks);
    indices.push_back(((j+1)*stacks)+1);
    indices.push_back((j*stacks)+1);
  }

  int initialSouthP = (((slices-1)*stacks)+1) ;
  int sCenter=(slices*stacks)+2;
  // South Pole
  for(int i=initialSouthP;i<(sCenter-1);i++){
    indices.push_back(sCenter);
    indices.push_back(i);
    indices.push_back(i+1);
  }
  indices.push_back(sCenter);
  indices.push_back(initialSouthP+stacks);
  indices.push_back(initialSouthP);

  bindVertexArrayObjects(geometrySphere,vertexdata,indices);

}

/// creates a cube with length a
void createCube(float a)
{
  std::vector<Vertex> vertexdata ;
  std::vector<unsigned short> indices;

  glm::vec2 t;
  glm::vec3 n;
  glm::vec3 p;

  unsigned short index = 0;
  t = glm::vec2(0.0f,0.0f);
  p = glm::vec3(a,a,-a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);
  t =glm::vec2(0.0f,1.0f);
  p =glm::vec3(-a,a,-a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);
  t =glm::vec2(1.0f,1.0f);
  p =glm::vec3(-a,-a,-a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);
  t =glm::vec2(1.0f,0.0f);
  p =glm::vec3(a,-a,-a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);

  // +Z

  t = glm::vec2(0.0f,0.0f);
  p =glm::vec3(a,a,a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);
  t =glm::vec2(0.0f,1.0f);
  p =glm::vec3(a,-a,a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);
  t =glm::vec2(1,1);
  p =glm::vec3(-a,-a,a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);
  t =glm::vec2(1,0);
  p =glm::vec3(-a,a,a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);


  // +X
  t = glm::vec2(0.0f,0.0f);
  p =glm::vec3(a,a,-a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);
  t =glm::vec2(0.0f,1.0f);
  p =glm::vec3(a,-a,-a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);
  t =glm::vec2(1.0f,1.0f);
  p =glm::vec3(a,-a,a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);
  t =glm::vec2(1.0f,0.0f);
  p =glm::vec3(a,a,a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);

  // -X
  t = glm::vec2(0.0f,0.0f);
  p =glm::vec3(-a,a,-a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);
  t =glm::vec2(0.0f,1.0f);
  p =glm::vec3(-a,a,a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);
  t =glm::vec2(1.0f,1.0f);
  p =glm::vec3(-a,-a,a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);
  t =glm::vec2(1.0f,0.0f);
  p =glm::vec3(-a,-a,-a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);

  // +Y
  t = glm::vec2(0.0f,0.0f);
  p =glm::vec3(-a,a,-a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);
  t =glm::vec2(0.0f,1.0f);
  p =glm::vec3(a,a,-a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);
  t =glm::vec2(1.0f,1.0f);
  p =glm::vec3(a,a,a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);
  t =glm::vec2(1.0f,0.0f);
  p =glm::vec3(-a,a,a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);

  // +Y
  t = glm::vec2(0.0f,0.0f);
  p = glm::vec3(-a,-a,-a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);
  t =glm::vec2(0.0f,1.0f);
  p = glm::vec3(-a,-a,a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);
  t = glm::vec2(1.0f,1.0f);
  p = glm::vec3(a,-a,a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);
  t = glm::vec2(1.0f,0.0f);
  p = glm::vec3(a,-a,-a);
  vertexdata.push_back(Vertex(p,n,t));
  indices.push_back(index++);


  bindVertexArrayObjects(geometryCube,vertexdata,indices);


}

/// TODO
/// loads shuttle data from off-file and fills according GeometryData
void createShuttle()
{
  objA = new OffObject("../data/shuttle.off");


  std::vector<Vertex> vertexdata ;        // store the vertices of the shuttle here
  std::vector<unsigned short> indices;    // store the according indices here


  geometryShuttle.numVertices = objA->noOfVertices;
  geometryShuttle.numIndices = objA->noOfFaces*3;

  glm::vec3 p;
  glm::vec3 n;
  glm::vec2 t(0.0f,0.0f);

  // TODO: Fill vertexdata

  for (unsigned int i=0; i< geometryShuttle.numVertices; i++)
  {
    p=objA->vertexList[i];
    n=objA->normalsList[i];
    vertexdata.push_back(Vertex (p,n,t));
  }

  // TODO: Fill indexData

  for (int i=0; i< objA->noOfFaces; i++)
  {
    indices.push_back(objA->faceList[i].A);
    indices.push_back(objA->faceList[i].B);
    indices.push_back(objA->faceList[i].C);
  }

  bindVertexArrayObjects(geometryShuttle,vertexdata,indices);
}

void initTexture(GLint name, GLint w, GLint h, GLubyte *data) {

  glBindTexture(GL_TEXTURE_2D, name);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, w,h,0,GL_RGB,GL_UNSIGNED_BYTE, data);
}



void initGL() {

  glClearColor(0,0,0,0);   // set the clear color to black
  glEnable(GL_DEPTH_TEST); // turn on the depth test
  glEnable(GL_CULL_FACE);  // turn on backface culling

  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);


  // set the camera:

  glm::vec3 eye = glm::vec3( cam.position);
  glm::vec3 center= glm::vec3( cam.position + cam.viewDir);
  glm::vec3 up= glm::vec3( cam.upDir);
  V = glm::lookAt(eye,center,up);


  // create the shaders (the functions are defined in helper.h)


  // create the shaders (the functions are defined in helper.h)
  createProgram_VF("sun_VS.glsl","sun_FS.glsl",&SunShader.Shader);
  createProgram_VF("LightandTex_VS.glsl","LightandTex_FS.glsl",&TexturePhongShader.Shader);


  // ***********************************************************************************
  // You can add texture creation from previous assignment


  //MyImage earth, moon, saturn, back;
  // Use the code from your previous solutions!

  GLuint textureID[4];
  glGenTextures(4, textureID);

  MyImage earth;
  earthTex=textureID[0];
  earth.load("../data/earth.jpg");
  initTexture(earthTex,1000,500,earth.pixels);

  MyImage moon;
  moonTex=textureID[1];
  moon.load("../data/moon.jpg");
  initTexture(moonTex,1000,500,moon.pixels);

  MyImage saturn;
  saturnTex=textureID[2];
  saturn.load("../data/saturn.jpg");
  initTexture(saturnTex,1800,900,saturn.pixels);

  MyImage background;
  backgroundTex=textureID[3];
  background.load("../data/background.jpg");
  initTexture(backgroundTex,512,512,background.pixels);

  // ***********************************************************************************


  // the space shuttle & other geometry:
  createCube(300);
  createShuttle();
  createSphere();
  createRings();

}

void reshape(int w, int h)
{
  glViewport(0,0,(GLsizei) w, (GLsizei) h);

  P = glm::perspective(70.0f, (GLfloat) w/ (GLfloat) h, 10.0f, 800.0f);

}


void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(SunShader.Shader);


  // create Matrices
  M = glm::rotate(50.f, glm::vec3(0.f,1.f,0.f));

  glm::vec4 LightSource = V * glm::vec4(0,0,0,1)  ;
  glm::vec4 Color (0.9,0.9,0, 1); // set the color to yellow

  // bind Shader & bind Uniforms
  SunShader.bindUniforms(M,V,P, LightSource, Color,0, t);

  glutSolidSphere(25,150,80);


  glUseProgram(TexturePhongShader.Shader);
  /*
  GLuint vao;
  unsigned int numVertices;
  unsigned int numIndices;
  */

  //Space Cube
  M = glm::mat4();
  Color = glm::vec4(0.9,0.1,0.9,0.0);
  TexturePhongShader.bindUniforms(M,V,P, LightSource, Color,backgroundTex,t);
  glBindVertexArray(geometryCube.vao);//Bind VAO
  glDrawElements(GL_QUADS, geometryCube.numIndices, GL_UNSIGNED_SHORT, (void*)0);

  // the shuttle:
  Color = glm::vec4(0.1,0.1,0.1,1.0); // Set an appropriate color ;)

  M = glm::rotate(glm::radians(t/2.0f), glm::vec3(0.f,0.f,-1.f));
  M = glm::translate(M, glm::vec3(0.f,-100.f,0.f));
  M = glm::scale(M, glm::vec3(1.0f,-1.0f,1.0f));

  TexturePhongShader.bindUniforms(M,V,P, LightSource, Color,0,t);
  glBindVertexArray(geometryShuttle.vao);//Bind VAO
  glDrawElements(GL_TRIANGLES, geometryShuttle.numIndices, GL_UNSIGNED_SHORT, (void*)0);

  // the earth:
  Color = glm::vec4(0.0,0.0,1.0,1.0); // Set an appropriate color ;)

  M = glm::rotate(glm::radians(t), glm::vec3(0.f,1.f,0.f));
  M = glm::translate(M, glm::vec3(50.f,0.f,0.f));
  glm::mat4 Mearth= glm::scale(M, glm::vec3(7.0f,7.0f,7.0f));

  TexturePhongShader.bindUniforms(Mearth,V,P, LightSource, Color,earthTex,t);
  glBindVertexArray(geometrySphere.vao);//Bind VAO
  glDrawElements(GL_TRIANGLES, geometrySphere.numIndices, GL_UNSIGNED_SHORT, (void*)0);

  // the moon;
  Color = glm::vec4(0.5,0.5,0.5,1.0);
  M = glm::rotate(M,glm::radians(t*2.f), glm::vec3(1.f,0.f,0.f));
  M = glm::translate(M ,glm::vec3(0.f,10.f,0.f));

  TexturePhongShader.bindUniforms(M,V,P, LightSource, Color,moonTex,t);
  glBindVertexArray(geometrySphere.vao);//Bind VAO
  glDrawElements(GL_TRIANGLES, geometrySphere.numIndices, GL_UNSIGNED_SHORT, (void*)0);

  // Saturn:
  Color = glm::vec4(1.0,0.0,0.0,1.0);
  M = glm::rotate(glm::radians(t/2.0f), glm::vec3(0.f,1.f,0.f));
  M = glm::translate(M, glm::vec3(100.f,0.f,0.f));
  glm::mat4 Msaturn = glm::scale(M, glm::vec3(12.0f,12.0f,12.0f));

  TexturePhongShader.bindUniforms(Msaturn,V,P, LightSource, Color,saturnTex,t);
  glBindVertexArray(geometrySphere.vao);//Bind VAO
  glDrawElements(GL_TRIANGLES, geometrySphere.numIndices, GL_UNSIGNED_SHORT, (void*)0);

  M = glm::rotate(M, glm::radians(30.f), glm::vec3(0.f,0.f,1.f));
  M = glm::scale(M, glm::vec3(18.0f,18.0f,18.0f));

  glBindVertexArray(geometryRings.vao);//Bind VAO
  for (int i=0;i<30; i++) {
    Color = glm::vec4 (0.8-(i%3)*0.2,0.7-(i%3)*0.2,0.1,1.0);
    M = glm::scale(M, glm::vec3(1.01f,1.01f,1.01f));
    TexturePhongShader.bindUniforms(M,V,P, LightSource,Color, 0, t);
    glDrawElements(GL_LINE_LOOP, geometryRings.numIndices, GL_UNSIGNED_SHORT, (void*)0);
  }

  // ***********************************************************************************
  // Add rest of solar system  from previous assignments

  //// TODO: Use perviously created geometry
  // use vertex array objects instead of immediate mode

  // TODO: redraw the same sphere geometry, modify the planets size via its model matrix

  // TODO: @saturn rings; redraw the same ring, modify the rings diameter via its model matrix

  // ***********************************************************************************


  //	glBindVertexArray(/*vao*/);
  //	glDrawElements(/*PRIMITIVE TYPE*/, /*NUMBER OF ELEMENTS*/, /*FORMAT of INDICES*/, (void*)0);
  //  glBindVertexArray(0);

  // ***********************************************************************************
  // TODO: Add space shuttle
  // Bonus: Compute elliptical orbit

  // ***********************************************************************************




  glBindVertexArray(0);//Unbind
  glUseProgram(0);
  glutSwapBuffers();

}

void cleanUp() {

  glDeleteTextures(1,&earthTex);
  glDeleteTextures(1,&moonTex);
  glDeleteTextures(1,&saturnTex);
  glDeleteTextures(1,&backgroundTex);

  glDeleteProgram(SunShader.Shader);
  glDeleteProgram(TexturePhongShader.Shader);

  glDeleteProgram(SunShader.Shader);
  glDeleteProgram(TexturePhongShader.Shader);

  glDeleteVertexArrays(1, &geometryShuttle.vao);
  glDeleteVertexArrays(1, &geometryCube.vao);
  glDeleteVertexArrays(1, &geometrySphere.vao);

}


void updateCamera() {
  glm::vec3 eye = glm::vec3( cam.position);
  glm::vec3 center= glm::vec3( cam.position + cam.viewDir);
  glm::vec3 up= glm::vec3( cam.upDir);
  V = glm::lookAt(eye,center,up);

  glutPostRedisplay();

}


void onIdle() {

  t+= speed;  // increase the time parameter

  glutPostRedisplay();
}

void onMouseDown(int button, int state, int x, int y) {

  currentX = x;
  currentY = y;

}


void onMouseMove(int x, int y) {

  float dx = currentX-x;
  float dy = currentY-y;

  cam.yaw(dx);
  cam.pitch(dy);

  currentX = x;
  currentY = y;

  updateCamera();
}


void keyboard(unsigned char key, int x, int y) {

  switch(key) {

    case 27:
    cleanUp();
    exit(1);
    break;
    case 'w': // move forward
    cam.moveForward(1);
    updateCamera();
    break;
    case 's': // move forward
    cam.moveBackward(1);
    updateCamera();
    break;
    case 'a':
    cam.roll(-1.0);
    updateCamera();
    break;
    case 'd':
    cam.roll(1.0);
    updateCamera();
    break;

    case 'p': // toggle polygon mode:
    wireframe_mode = !wireframe_mode;
    if (wireframe_mode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    break;


    // increase / decrease the speed of the point light P:
    case '+':
    speed += 0.01;
    break;
    case '-':
    speed -= 0.01;
    break;
  }


  glutPostRedisplay();
}


int main(int argc, char **argv)
{

  glutInit(&argc, argv);
  glutInitWindowSize(1042, 1024);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

  glutCreateWindow("Planet System");
  GLenum err = glewInit();

  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMotionFunc(onMouseMove);
  glutMouseFunc(onMouseDown);
  glutReshapeFunc(reshape);
  glutIdleFunc(onIdle);
  initGL();
  glutMainLoop();

}
