#version 330

uniform mat4 transformation;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 inColor;

out vec3 vertColor;

void main()
{
	gl_Position = transformation * vec4(position, 1.0);
	vertColor = inColor;
}