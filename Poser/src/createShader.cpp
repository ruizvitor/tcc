#include "createShader.hpp"

class Shader;

// Constructor generates the shader on the fly
Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
  const GLchar* geometryPath=nullptr;
  // 1. Retrieve the vertex/fragment source code from filePath
  std::string vertexCode;
  std::string fragmentCode;
  std::string geometryCode;
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;
  std::ifstream gShaderFile;
  // ensures ifstream objects can throw exceptions:
  vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
  gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
  try
  {
    // Open files
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    std::stringstream vShaderStream, fShaderStream;
    // Read file's buffer contents into streams
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    // close file handlers
    vShaderFile.close();
    fShaderFile.close();
    // Convert stream into string
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
    // If geometry shader path is present, also load a geometry shader
    if(geometryPath != nullptr)
    {
      gShaderFile.open(geometryPath);
      std::stringstream gShaderStream;
      gShaderStream << gShaderFile.rdbuf();
      gShaderFile.close();
      geometryCode = gShaderStream.str();
    }
  }
  catch (std::ifstream::failure e)
  {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
  }
  const GLchar* vShaderCode = vertexCode.c_str();
  const GLchar * fShaderCode = fragmentCode.c_str();
  // 2. Compile shaders
  GLuint vertex, fragment;
  // GLint success;
  // GLchar infoLog[512];
  // Vertex Shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);
  checkCompileErrors(vertex, "VERTEX");
  // Fragment Shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, NULL);
  glCompileShader(fragment);
  checkCompileErrors(fragment, "FRAGMENT");
  // If geometry shader is given, compile geometry shader
  GLuint geometry=0;
  if(geometryPath != nullptr)
  {
    const GLchar * gShaderCode = geometryCode.c_str();
    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &gShaderCode, NULL);
    glCompileShader(geometry);
    checkCompileErrors(geometry, "GEOMETRY");
  }
  // Shader Program
  this->Program = glCreateProgram();
  glAttachShader(this->Program, vertex);
  glAttachShader(this->Program, fragment);
  if(geometryPath != nullptr)
  {
    glAttachShader(this->Program, geometry);
  }
  glLinkProgram(this->Program);
  checkCompileErrors(this->Program, "PROGRAM");
  // Delete the shaders as they're linked into our program now and no longer necessery
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  if(geometryPath != nullptr)
  {
    glDeleteShader(geometry);
  }

}
// Uses the current shader
void Shader::Use() { glUseProgram(this->Program); }
void Shader::BindMatrices(glm::mat4& MV,glm::mat4* P)
{
  glm::mat4 MVP= (*P) * MV;
  glm::mat3 NormalMatrix	= glm::transpose(glm::inverse(glm::mat3(MV)));

  //Bind MVP to shader
  glUniformMatrix3fv(glGetUniformLocation(this->Program, "u_NormalMatrix"), 1, GL_FALSE, glm::value_ptr(NormalMatrix));
  glUniformMatrix4fv(glGetUniformLocation(this->Program, "u_MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
  glUniformMatrix4fv(glGetUniformLocation(this->Program, "u_MV"), 1, GL_FALSE, glm::value_ptr(MV));
  // glUniformMatrix4fv(glGetUniformLocation(this->Program, "u_M"), 1, GL_FALSE, glm::value_ptr(*M));
}
void Shader::BindMatrices(glm::mat4* M,glm::mat4* V,glm::mat4* P)
{
  glm::mat4 MV= (*V) * (*M);
  glm::mat4 MVP= (*P) * MV;
  glm::mat3 NormalMatrix	= glm::transpose(glm::inverse(glm::mat3(MV)));

  //Bind MVP to shader
  glUniformMatrix3fv(glGetUniformLocation(this->Program, "u_NormalMatrix"), 1, GL_FALSE, glm::value_ptr(NormalMatrix));
  glUniformMatrix4fv(glGetUniformLocation(this->Program, "u_MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
  glUniformMatrix4fv(glGetUniformLocation(this->Program, "u_MV"), 1, GL_FALSE, glm::value_ptr(MV));
  // glUniformMatrix4fv(glGetUniformLocation(this->Program, "u_M"), 1, GL_FALSE, glm::value_ptr(*M));
}
void Shader::BindMatrices(glm::mat4* M,glm::mat4* V,glm::mat4* P, glm::vec3* lightPos)
{
  glm::mat4 MV= (*V) * (*M);
  glm::mat4 MVP= (*P) * MV;
  glm::mat3 NormalMatrix	= glm::transpose(glm::inverse(glm::mat3(MV)));

  //Bind MVP to shader
  glUniformMatrix3fv(glGetUniformLocation(this->Program, "u_NormalMatrix"), 1, GL_FALSE, glm::value_ptr(NormalMatrix));
  glUniformMatrix4fv(glGetUniformLocation(this->Program, "u_MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
  glUniformMatrix4fv(glGetUniformLocation(this->Program, "u_MV"), 1, GL_FALSE, glm::value_ptr(MV));
  // glUniformMatrix4fv(glGetUniformLocation(this->Program, "u_M"), 1, GL_FALSE, glm::value_ptr(*M));
  glUniform3fv(glGetUniformLocation(this->Program, "u_lightPosition"), 1, glm::value_ptr(*lightPos));
}

void Shader::checkCompileErrors(GLuint shader, std::string type)
{
  GLint success;
  GLchar infoLog[1024];
  if(type != "PROGRAM")
  {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      std::cout << "| ERROR::::SHADER-COMPILATION-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << std::endl;
    }
  }
  else
  {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if(!success)
    {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      std::cout << "| ERROR::::PROGRAM-LINKING-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << std::endl;
    }
  }
}

// ------------------------------------------------------------------------
void Shader::setInt(const std::string &name, int value)
{
    glUniform1i(glGetUniformLocation(Program, name.c_str()), value);
}
// ------------------------------------------------------------------------
