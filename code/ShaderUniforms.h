#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
