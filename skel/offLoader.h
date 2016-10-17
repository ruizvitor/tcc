// Put the names of all group members here
// Daniel Vitor Ruiz - Matrikelnummer: 21983193 - Benutzerkennung: yx17upyj
// Joao Victor Tozatti Risso - Matrikelnummer: 21983403 - Benutzerkennung: ih94yxad

#pragma once
#include <fstream>
#include <vector>

#include <glm/glm.hpp>
#include "helper.h"

using namespace std;

// simple structure to hold three integer-values (used for triangle indicies)
struct tri {
  int A;
  int B;
  int C;
};


// Declaration:

class OffObject
{

public:

  vector<glm::vec3> vertexList;
  vector<glm::vec3> normalsList;
  vector<tri> faceList;

  int noOfFaces;
  int noOfVertices;

  OffObject(string filename);

};



//TODO: IMPLEMENTATION:

// the constuctor recieves the filename (an .off file) and parses it. The vertices, normals and triangles
// are pushed back into the respective container whereas the NORMALS have to be explicitly computed for each
// vertex.

OffObject::OffObject(string filename) {

  std::ifstream inFile(filename.c_str());
  char tmp[20];

  inFile >> tmp;
  inFile >> noOfVertices;
  inFile >> noOfFaces;
  inFile >> tmp;

  // Read Vertex Data and initialize the normals:
  for (int i=0; i<noOfVertices; i++)
  {
    glm::vec3 vertex;
    glm::vec3 normal(0.0f,0.0f,0.0f);
    // TODO

    // initalize the normal with (0,0,0)

    // add vertex and normal
    inFile >> vertex.x;
    inFile >> vertex.y;
    inFile >> vertex.z;
    vertexList.push_back(vertex);
    normalsList.push_back(normal);
  }

  glm::vec3 normaltotal(0.0f,0.0f,0.0f);
  // Read Triangle Data:
  for (int i=0; i<noOfFaces; i++)
  {
    tri T;
    glm::vec3 U;
    glm::vec3 V;
    // TODO
    inFile >> tmp;
    inFile >> T.A;
    inFile >> T.B;
    inFile >> T.C;
    faceList.push_back(T);
  }
  for (int i=0; i<(noOfVertices-1); i++)
  {
    normaltotal.x = normaltotal.x + ((vertexList[i].y - vertexList[i+1].y) * (vertexList[i].z + vertexList[i+1].z));
    normaltotal.y = normaltotal.y + ((vertexList[i].z - vertexList[i+1].z) * (vertexList[i].x + vertexList[i+1].x));
    normaltotal.z = normaltotal.z + ((vertexList[i].x - vertexList[i+1].x) * (vertexList[i].y + vertexList[i+1].y));
  }
  glm::normalize(normaltotal);
  for (int i=0; i<noOfVertices; i++)
  {
    normalsList[i]=normaltotal;
  }

}
