#version 460 core

layout (location = 0) in vec3 position;

out vec2 uv;

void main(){
	gl_Position = vec4(position, 1);
	uv = position.xy;
}