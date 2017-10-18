#ifndef _MESH_H_
#define _MESH_H_
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// Own Library
#include "createShader.hpp"
// 3dAssets Handler
#include "../libs/assimp/Importer.hpp"
#include "../libs/assimp/scene.h"
#include "../libs/assimp/postprocess.h"
// #include <assimp/Importer.hpp>
// #include <assimp/scene.h>
// #include <assimp/postprocess.h>

using namespace std;

struct Vertex {
  // Position
  glm::vec3 Position;
  // Normal
  glm::vec3 Normal;
  // TexCoords
  glm::vec2 TexCoords;
  // Tangent
  glm::vec3 Tangent;
  // Bitangent
  glm::vec3 Bitangent;
};

struct Texture {
  GLuint id;
  string type;
  aiString path;
};

class Mesh {
public:
  /*  Mesh Data  */
  vector<Vertex> vertices;
  vector<GLuint> indices;
  vector<Texture> textures;
  GLuint VAO;
  // float xmin;
  // float xmax;
  // float ymin;
  // float ymax;

  /*  Functions  */
  // Constructor
  Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);

  // Render the mesh
  void Draw(Shader &shader);

private:
  /*  Render data  */
  GLuint VBO, EBO;

  /*  Functions    */
  // Initializes all the buffer objects/arrays
  void setupMesh();
};
#endif // _MESH_H_
