#version 460 core

const vec2 invAtan = vec2(0.1591, 0.3183);

in vec3 cubeUV;
out vec4 FragColor;

uniform sampler2D environmentMap;

vec2 CubeUVToEnvironmentUV(vec3 v);

void main(){
	FragColor = vec4(texture(environmentMap, CubeUVToEnvironmentUV(cubeUV)).rgb, 1);
}
vec2 CubeUVToEnvironmentUV(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}