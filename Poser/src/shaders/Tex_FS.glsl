#version 330 core

in vec2 o_TexCoords;

layout(location = 0) out vec3 resultingColor;

uniform sampler2D texture_diffuse1;

void main()
{
  vec3 texColor = texture(texture_diffuse1, o_TexCoords).rgb;
  resultingColor = texColor;
  // resultingColor = (1.0,0.0,0.0);
}
