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
struct HitInfo{
	bool didHit;
	float t;

	vec3 pos;
	vec3 normal;
};

struct MathSphere{
	vec3 position;
	float radius;
};

const HitInfo NoHit = HitInfo(false, 0, vec3(0), vec3(0));

vec3 At(Ray ray, float t);

HitInfo Hit_MathSphere(MathSphere mathSphere, Ray ray);

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

	HitInfo hitInfo = Hit_MathSphere(mathSpheres[0], ray);

	if (hitInfo.didHit) FragColor = vec4(hitInfo.normal, 1);
	else FragColor = vec4(0, 0.5, 0.3, 1);
}
vec3 At(Ray ray, float t){
	return ray.origin + ray.direction * t;
};

HitInfo Hit_MathSphere(MathSphere mathSphere, Ray ray){
	vec3 oc = ray.origin - mathSphere.position;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - mathSphere.radius*mathSphere.radius;
    float discriminant = b*b - 4*a*c;

    if (discriminant < 0) return NoHit;
	else {
		float t = (-b - sqrt(discriminant) ) / (2.0*a);

		if (t < 0) return NoHit;
		
		vec3 hitPos = At(ray, t);
		return HitInfo(true, t, hitPos, (hitPos - mathSphere.position) / mathSphere.radius);
	}
};