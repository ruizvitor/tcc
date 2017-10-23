#ifndef _MODEL_H_
#define _MODEL_H_
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// Image Handler
#include <opencv2/opencv.hpp>
// 3dAssets Handler
#include "../libs/assimp/Importer.hpp"
#include "../libs/assimp/scene.h"
#include "../libs/assimp/postprocess.h"
// #include <assimp/Importer.hpp>
// #include <assimp/scene.h>
// #include <assimp/postprocess.h>
//
#include "mesh.hpp"

using namespace std;
using namespace cv;

class Model
{
public:
  /*  Model Data */
  vector<Texture> textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
  vector<Mesh> meshes;
  string directory;
  bool gammaCorrection;

  /*  Functions   */
  // Constructor, expects a filepath to a 3D model.
  // Model(string const & path, bool gamma = false) : gammaCorrection(gamma)
  // {
  //   this->loadModel(path);
  //   cout << "Model Load Complete" << endl;
  // }

  // Draws the model, and thus all its meshes
  void Draw(Shader shader);

  // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
  void loadModel(string path);

private:
  /*  Functions   */
  // Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
  void processNode(aiNode* node, const aiScene* scene);

  Mesh processMesh(aiMesh* mesh, const aiScene* scene);

  // Checks all material textures of a given type and loads the textures if they're not loaded yet.
  // The required info is returned as a Texture struct.
  vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};

// template<typename T1,typename T2,typename T3>
// void flipImg(T1 &old, T2 &res, T3 m, T3 n);
//
// template<typename T1,typename T2,typename T3>
// void flipImgAlpha(T1 &old, T2 &res, T3 m, T3 n);

void flipImg(unsigned char* old, std::vector< unsigned char >& res, int m, int n);

GLint TextureFromFile(const char* path, string directory, bool gamma = false);
GLint TextureFromFile(const char* path, string directory, bool gamma);

#endif // _MODEL_H_
