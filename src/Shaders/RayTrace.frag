#version 460 core

struct Camera{
	vec3 position;
	vec3 xAxis;
	vec3 yAxis;
	vec3 zAxis;
	float focalLength;
	uint viewPortWidth;
	uint viewPortHeight;
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

	vec3 albedo;

	float roughness;
	float metalness;
};

struct MathSphere{
	vec3 position;
	float radius;
	vec3 color;
	float roughness;
	float metalness;
};

// Constants
const float PI = 3.14159265359;
const HitInfo NoHit = HitInfo(false, 1. / 0., vec3(0), vec3(0), vec3(0), 0, 0);

// Helper functions
float Rand();
vec3 At(Ray ray, float t);
vec3 SampleEnvironmentMap(vec3 direction);
vec3 SampleHemisphere(vec3 normal);
mat3 GetTangentSpace(vec3 normal);
float sdot(vec3 v0, vec3 v1);

vec3 GGXComputeRadiance(vec3 wo, HitInfo hitInfo, out vec3 newDir);
vec3 GGXImportanceSampleHemisphere(vec3 N, vec3 wo, float t, float roughness);
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

// Intersection functions
HitInfo Hit_MathSphere(MathSphere mathSphere, Ray ray);

in vec2 uv;

layout (location = 0) out vec3 CumulativeColor;
layout (location = 1) out vec3 FinalColor;

uniform sampler2D cumulativeRenderTexture;

uniform uint currSample;

uniform uint seed;
uint _seed;

uniform sampler2D environmentMap;

uniform Camera camera;
uniform MathSphere mathSpheres[10];
uniform uint maxDepth;

void main(){
	_seed = seed;

	vec3 worldUV = camera.position + 
	-camera.zAxis * camera.focalLength + 
	camera.xAxis * float(camera.viewPortWidth) / 2.0 * uv.x +
	camera.yAxis * float(camera.viewPortHeight) / 2.0 * uv.y;

	vec3 radiance = vec3(1.0);

	Ray ray = Ray(camera.position, normalize(worldUV - camera.position));

	for (uint depth = 1; depth <= maxDepth + 1; depth++){
		if (depth == maxDepth + 1){
			radiance = vec3(0.0);
			break;
		}
		HitInfo closestHit = NoHit;

		for (uint i = 0; i < mathSpheres.length; i++){
			HitInfo hitInfo = Hit_MathSphere(mathSpheres[i], ray);
			if (hitInfo.didHit && hitInfo.t < closestHit.t) closestHit = hitInfo;
		}
		if (closestHit.didHit){
			vec3 wi;

			radiance *= GGXComputeRadiance(ray.direction, closestHit, wi);

			ray.origin = closestHit.position + closestHit.normal * 0.001;
			ray.direction = wi;
		}
		else{
			radiance *= SampleEnvironmentMap(ray.direction);
			break;
		}
	}
	CumulativeColor = texture(cumulativeRenderTexture, (uv + 1.0) / 2.0).rgb + radiance;
	FinalColor = CumulativeColor / float(currSample);
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
		return HitInfo(true, t, hitPos, (hitPos - mathSphere.position) / mathSphere.radius, mathSphere.color, mathSphere.roughness, mathSphere.metalness);
	}
};

float Rand(){
	const float MAXHASH = 4294967295.0;

	vec2 pixel = vec2((uv.x + 1.0) / 2.0 * camera.viewPortWidth, (uv.y + 1.0) / 2.0 * camera.viewPortHeight);

    uint value = uint(pixel.y * camera.viewPortWidth + pixel.x);
    value *= _seed;
    value ^= 2747636419u;
    value *= 2654435769u;
    value ^= value >> 16;
    value *= 2654435769u;
    value ^= value >> 16;
    value *= 2654435769u;
    _seed++;
    return float(value) / MAXHASH;
}
float sdot(vec3 v0, vec3 v1){
	return max(0.0, dot(v0, v1));
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

vec3 GGXImportanceSampleHemisphere(vec3 N, vec3 wo, float t, float roughness){

	float e0 = Rand();
	float e1 = Rand();
	float e2 = Rand();

	float theta;

	if (e0 > t) theta = acos(sqrt(e1));
	else theta = atan((roughness * sqrt(e1))/sqrt(1 - e1));

	float phi = 2.0 * PI * e2;

	vec3 sampleVector = normalize(GetTangentSpace(N) * vec3(sin(theta)*cos(phi), sin(theta)*sin(phi),cos(theta)));
    
	if (e0 < t) sampleVector = reflect(wo, sampleVector);
	
	return sampleVector;
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
vec3 FresnelSchlickRoughness(float HdotV, vec3 F0, float alpha)
{
    return F0 + (max(vec3(1.0 - alpha), F0) - F0) * pow(clamp(1.0 - HdotV, 0.0, 1.0), 5.0);
}
vec3 FresnelSchlick(float HdotV, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - HdotV, 0.0, 1.0), 5.0);
}
float DistributionGGX(vec3 N, vec3 H, float alpha)
{
//    float a      = alpha*alpha;
//    float a2     = a*a;
//    float NdotH  = sdot(N, H);
//    float NdotH2 = NdotH*NdotH;
//	
//    float num   = a2;
//    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
//    denom = PI * denom * denom;
//	
//    return num / denom;

	float NoH = sdot(N,H);
    float alpha2 = alpha * alpha;
    float NoH2 = NoH * NoH;
    float den = NoH2 * alpha2 + (1 - NoH2);
    return alpha2 / ( PI * den * den );
}

float GeometrySchlickGGX(float NdotV, float alpha)
{
    float r = (alpha + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float alpha)
{
    float NdotV = sdot(N, V);
    float NdotL = sdot(N, L);
    float ggx2  = GeometrySchlickGGX(NdotV, alpha);
    float ggx1  = GeometrySchlickGGX(NdotL, alpha);
	
    return ggx1 * ggx2;
}
vec3 GGXComputeRadiance(vec3 wo, HitInfo hitInfo, out vec3 wi){
	const float t = 0.25;

	float alpha = clamp(hitInfo.roughness, 0.001, 0.9999);
	hitInfo.metalness = clamp(hitInfo.metalness, 0.0, 1.0);

	vec3 N = hitInfo.normal;
	vec3 V = normalize(camera.position - hitInfo.position);

	wi = GGXImportanceSampleHemisphere(hitInfo.normal, wo, t, alpha);
	
	if (dot(wi, N) < 0) return vec3(0);

	vec3 H = normalize(wi + V);

	vec3 lambert = hitInfo.albedo / PI;

	float D = DistributionGGX(N, H, alpha);
			
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, hitInfo.albedo, hitInfo.metalness);
	vec3 F = FresnelSchlickRoughness(sdot(H, V), F0, alpha);
			
	float G = GeometrySmith(N, V, wi, alpha);

	vec3 cookTorrence = D*F*G/(4.0 * sdot(N, V) * sdot(N, wi) + 0.0001);

	vec3 kS = F;
	vec3 kD = (vec3(1.0) - kS) * (1.0 - hitInfo.metalness);

	vec3 BRDF = kD * lambert + kS * cookTorrence;
	float pdf = (1 - t) * (sdot(hitInfo.normal, wi)/PI) + t * (DistributionGGX(N, H, alpha)/4*sdot(wi, H));

	return BRDF / pdf * sdot(N, wi);
};
