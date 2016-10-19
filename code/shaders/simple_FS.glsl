#version 330 core

in vec2 TexCoords;
in vec3 Normals;

out vec4 color;

uniform sampler2D texture_diffuse1;

void main()
{
    // color = vec4(texture(texture_diffuse1, TexCoords))*vec4(Normals,1);
    // color = vec4(texture(texture_diffuse1, TexCoords));
    color = vec4(Normals,1)+vec4(0.3,0.3,0.3,1.0);
}
