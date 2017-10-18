#version 330 core
layout(location = 0) out vec3 resultingColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    resultingColor = col;
    // resultingColor = vec3(TexCoords,0.0);
    // resultingColor = vec3(1.0,0.0,0.0);
    // FragColor = vec4(vec3(1.0,0.0,0.0), 1.0);
}
