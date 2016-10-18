// #version 330 core
attribute vec3 vertexPosition_modelspace;
// layout(location = 0) in vec3 vertexPosition_modelspace;
uniform mat4 MVP;

void main()
{
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1.0);
}
