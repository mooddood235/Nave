#version 460 core

in vec2 uv;
out vec3 FragColor;

uniform sampler2D colorTexture;

void main(){
	vec3 color = texture(colorTexture, (uv + 1.0) / 2.0).rgb;
	FragColor = color / (color + vec3(1.0));
}