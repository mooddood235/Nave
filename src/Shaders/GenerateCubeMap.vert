#version 460 core

layout (location = 0) in vec3 position;

out vec3 cubeUV;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(){
	gl_Position = projectionMatrix * viewMatrix * vec4(position, 1);
	cubeUV = position;
}