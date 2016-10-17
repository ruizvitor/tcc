#pragma once
#include "objLoader.h"
#include "vboindexer.h"
#include "GeometryData.h"

void createOBJ(GeometryData geometry,const char * path)
{
  // Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ(path, vertices, uvs, normals);
  if(!res)
  {
    printf("problem with OBJ parser");
    return;
  }

	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	indexVBO(vertices, uvs, normals, geometry.indices, indexed_vertices, indexed_uvs, indexed_normals);


	// Load it into a VBO
  // Create and bind a BO for vertex data
	glGenBuffers(1, &(geometry.vertexbuffer));
	glBindBuffer(GL_ARRAY_BUFFER, geometry.vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &(geometry.uvbuffer));
	glBindBuffer(GL_ARRAY_BUFFER, geometry.uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &(geometry.normalbuffer));
	glBindBuffer(GL_ARRAY_BUFFER, geometry.normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	glGenBuffers(1, &(geometry.elementbuffer));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry.elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, geometry.indices.size() * sizeof(unsigned short), &(geometry.indices[0]) , GL_STATIC_DRAW);
}
