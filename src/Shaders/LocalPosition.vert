#version 460 core

layout (location = 0) in vec3 pos;

out vec3 localPos;

void main(){
	gl_Position = vec4(pos, 1);
	localPos = pos;
}
