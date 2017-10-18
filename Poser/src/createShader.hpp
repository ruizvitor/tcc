#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
  GLuint Program;
  Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
  void Use();
  void setInt(const std::string &name, int value);
  void BindMatrices(glm::mat4& MV,glm::mat4* P);
  void BindMatrices(glm::mat4* M,glm::mat4* V,glm::mat4* P);
  void BindMatrices(glm::mat4* M,glm::mat4* V,glm::mat4* P, glm::vec3* lightPos);
private:
  void checkCompileErrors(GLuint shader, std::string type);
};

#endif//END SHADER_H
