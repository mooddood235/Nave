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

	vec3 position;
	vec3 normal;
};

struct MathSphere{
	vec3 position;
	float radius;
};

// Constants
const float PI = 3.14159265359;
const HitInfo NoHit = HitInfo(false, 1. / 0., vec3(0), vec3(0));

// Helper functions
float Rand(vec2 uv);
vec3 At(Ray ray, float t);
vec3 SampleEnvironmentMap(vec3 direction);
vec3 SampleHemisphere(vec3 normal);
mat3 GetTangentSpace(vec3 normal);

// Intersection functions
HitInfo Hit_MathSphere(MathSphere mathSphere, Ray ray);

in vec2 uv;
out vec4 FragColor;

uniform float seed;
float _seed;

uniform sampler2D environmentMap;
uniform Camera camera;
uniform MathSphere mathSpheres[10];
uniform unsigned int maxDepth;
uniform unsigned int maxSamples;


void main(){
	_seed = seed;

	vec3 worldUV = camera.position + 
	-camera.zAxis * camera.focalLength + 
	camera.xAxis * camera.viewPortWidth / 2.0 * uv.x +
	camera.yAxis * camera.viewPortHeight / 2.0 * uv.y;

	vec3 irradiance = vec3(0.0);

	for (unsigned int _sample = 1; _sample <= maxSamples; _sample++){
		vec3 radiance = vec3(1.0);

		Ray ray = Ray(camera.position, normalize(worldUV - camera.position));

		for (unsigned int depth = 1; depth <= maxDepth + 1; depth++){
			if (depth == maxDepth + 1){
				radiance = vec3(0.0);
				break;
			}
			HitInfo closestHit = NoHit;

			for (unsigned int i = 0; i < mathSpheres.length; i++){
				HitInfo hitInfo = Hit_MathSphere(mathSpheres[i], ray);
				if (hitInfo.didHit && hitInfo.t < closestHit.t) closestHit = hitInfo;
			}
			if (closestHit.didHit){
				vec3 reflectedDir = SampleHemisphere(closestHit.normal);
				radiance *= 2 * vec3(0.2,0.5,0.3) * max(0, dot(reflectedDir, closestHit.normal));
				ray = Ray(closestHit.position + reflectedDir * 0.001, reflectedDir);
			}
			else{
				radiance *= SampleEnvironmentMap(ray.direction);
				break;
			}
		}
		irradiance += radiance;
	}
	irradiance /= maxSamples;
	FragColor = vec4(irradiance / (irradiance + vec3(1.0)), 1);
}
vec3 At(Ray ray, float t){
	return ray.origin + ray.direction * t;
};
vec3 SampleEnvironmentMap(vec3 direction)
{
	const vec2 invAtan = vec2(0.1591, 0.3183);

    vec2 uv = vec2(atan(direction.z, direction.x), asin(direction.y));
    uv *= invAtan;
    uv += 0.5;
    
	return texture(environmentMap, uv).rgb;
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

float Rand(){
	float result = fract(sin(_seed / 100.0 * dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
    _seed += uv.x + uv.y * camera.viewPortWidth;
    return result;
};

vec3 SampleHemisphere(vec3 normal){
	// Uniformly sample hemisphere direction
    float cosTheta = Rand();
    float sinTheta = sqrt(max(0.0, 1.0 - cosTheta * cosTheta));
    float phi = 2 * PI * Rand();
    vec3 sampleVector = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
    // Transform direction to world space
    return GetTangentSpace(normal) * sampleVector;
};
mat3 GetTangentSpace(vec3 normal){
    // Choose a helper vector for the cross product
    vec3 helper = vec3(1, 0, 0);
    if (abs(normal.x) > 0.99) helper = vec3(0, 0, 1);
    // Generate vectors
    vec3 tangent = normalize(cross(normal, helper));
    vec3 binormal = normalize(cross(normal, tangent));
    return mat3(tangent, binormal, normal);
}