#version 330 core

in vec2 o_TexCoords;
in vec3 normalInterp;
in vec3 vertPos;
in vec3 lightPos;

layout(location = 0) out vec3 resultingColor;

uniform sampler2D texture_diffuse1;

// const vec3 lightPos = vec3(1.0,1.0,1.0);
const vec3 ambientColor = vec3(0.1, 0.2, 0.1);
const vec3 diffuseColor = vec3(0.2, 0.2, 0.2);
const vec3 specColor = vec3(0.3, 0.3, 0.3);

void main()
{
  vec3 normal = normalize(normalInterp);
  vec3 lightDir = normalize(lightPos - vertPos);
  vec3 reflectDir = reflect(-lightDir, normal);
  vec3 viewDir = normalize(-vertPos);

  float lambertian = max(dot(lightDir,normal), 0.0);
  float specular = 0.0;

  if(lambertian > 0.0) {
     float specAngle = max(dot(reflectDir, viewDir), 0.0);
     specular = pow(specAngle, 4.0);
  }

  vec3 texColor = diffuseColor;
  // texColor = texture(texture_diffuse1, o_TexCoords).rgb;

  resultingColor = ambientColor +
                   lambertian*texColor +
                   specular*specColor;
  // resultingColor = texColor;
}
