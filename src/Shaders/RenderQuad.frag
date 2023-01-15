#version 460 core
struct Camera{
	vec3 position;
	vec3 xAxis;
	vec3 yAxis;
	vec3 zAxis;
	float focalLength;
	float viewPortWidth;
	float viewPortHeight;
};
struct Ray{
	vec3 origin;
	vec3 direction;
};

struct MathSphere{
	vec3 position;
	float radius;
};

bool Hit_MathSphere(MathSphere mathSphere, Ray ray);

in vec2 uv;

out vec4 FragColor;

uniform Camera camera;

uniform MathSphere mathSpheres[1];

void main(){
	vec3 worldUV = camera.position + 
	-camera.zAxis * camera.focalLength + 
	camera.xAxis * camera.viewPortWidth / 2.0 * uv.x +
	camera.yAxis * camera.viewPortHeight / 2.0 * uv.y;

	Ray ray = Ray(camera.position, normalize(worldUV - camera.position));

	if (Hit_MathSphere(mathSpheres[0], ray)) FragColor = vec4(1, 0, 0, 1);
	else FragColor = vec4(0, 0, 0, 1);
}
bool Hit_MathSphere(MathSphere mathSphere, Ray ray){
	vec3 oc = ray.origin - mathSphere.position;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - mathSphere.radius*mathSphere.radius;
    float discriminant = b*b - 4*a*c;

    if (discriminant < 0) return false;
	else return (-b - sqrt(discriminant) ) / (2.0*a) > 0;
};