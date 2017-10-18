#version 330 core
layout (location = 0) in vec3 i_position;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_texCoords;

// data for fragment shader
out vec3 normalInterp;
out vec2 o_TexCoords;

// Transformation Matrices
uniform mat3 u_NormalMatrix;
uniform mat4 u_MV;
uniform mat4 u_MVP;

// position of light and camera
uniform vec3 u_lightPosition;

void main()
{
  o_TexCoords = i_texCoords;
  normalInterp = u_NormalMatrix * i_normal;
  gl_Position = u_MVP * vec4(i_position, 1.0f);
}
