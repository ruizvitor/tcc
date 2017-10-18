#version 330 core

in vec2 o_TexCoords;
in vec3 normalInterp;
// in vec3 vertPos;
// in vec3 lightPos;

layout(location = 0) out vec3 resultingColor;
layout(location = 1) out vec3 newUVS;

// uniform sampler2D texture_diffuse1;

// const vec3 lightPos = vec3(1.0,1.0,1.0);
// const vec3 ambientColor = vec3(0.2, 0.2, 0.2);
// const vec3 diffuseColor = vec3(0.3, 0.3, 0.3);
// const vec3 specColor = vec3(0.5, 0.5, 0.5);

void main()
{
  vec3 normal = normalize(normalInterp);
  resultingColor = normal;
  // resultingColor += 0.1*normal;
  // resultingColor = vec3(1,0,0);
  newUVS = vec3(o_TexCoords.y,o_TexCoords.x,0.0);
  // newUVS = o_TexCoords;
}
