#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

out vec3 localPos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(){
	localPos = pos;

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(pos, 1);
}
