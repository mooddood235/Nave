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

// GGX distribution
vec3 ComputeRadianceGGX(vec3 wo, HitInfo hitInfo, out vec3 wi);
float D_GGX(vec3 n, vec3 m, float a);
float G_GGX(vec3 n, vec3 x, float a);
float G_smith(vec3 n, vec3 l, vec3 v, float a);
vec3 F_schlick(vec3 v, vec3 h, vec3 F0);
vec3 ImportanceSampleGGX(vec3 n, float a);
vec3 ImportanceSampleGGXVNDF(vec3 N, vec3 V, float alpha);

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

			radiance *= ComputeRadianceGGX(ray.direction, closestHit, wi);

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
		return HitInfo(true, t, hitPos, normalize(hitPos - mathSphere.position), mathSphere.color, mathSphere.roughness, mathSphere.metalness);
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
    return normalize(GetTangentSpace(normal) * sampleVector);
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
float Den(float value){
	return max(0.0001, value);
}
float D_GGX(vec3 n, vec3 m, float a){	
	float a2 = a * a;

	float ndotm = sdot(n, m);
	float ndotm2 = ndotm * ndotm;

	float numerator = a2;
	float denominator = ndotm2 * (a2 - 1.0) + 1.0;
	denominator *= denominator;
	denominator *= PI;

	return numerator / Den(denominator);
};
float G_GGX(vec3 n, vec3 x, float a){
	float a2 = a * a;

	float ndotx = sdot(n, x);
	float ndotx2 = ndotx * ndotx;

	float numerator = 2.0 * ndotx;
	float denominator = ndotx + sqrt(a2 + (1.0 - a2) * ndotx2);

	return numerator / Den(denominator);
};
float G_smith(vec3 n, vec3 l, vec3 v, float a){
	return G_GGX(n, l, a) * G_GGX(n, v, a);
};

vec3 F_schlick(vec3 v, vec3 h, vec3 F0){
	float vdoth = sdot(v, h);

	return F0 + (1.0 - F0) * pow(1.0 - vdoth, 5.0);
};

vec3 ImportanceSampleGGX(vec3 n, float a){
	float e1 = Rand();
	float e2 = Rand();

	float theta = atan(a * sqrt(e1) / sqrt(1.0 - e1));
	float phi = 2.0 * PI * e2;

	return normalize(GetTangentSpace(n) * vec3(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta)));
};
vec3 ImportanceSampleGGXVNDF(vec3 n, vec3 v, float a)
{
	float U1 = Rand();
	float U2 = Rand();

	v = normalize(inverse(GetTangentSpace(n)) * v);

	// Section 3.2: transforming the view direction to the hemisphere configuration
	vec3 Vh = normalize(vec3(a * v.x, a * v.y, v.z));
	// Section 4.1: orthonormal basis (with special case if cross product is zero)
	float lensq = Vh.x * Vh.x + Vh.y * Vh.y;
	vec3 T1 = lensq > 0 ? vec3(-Vh.y, Vh.x, 0) * inversesqrt(lensq) : vec3(1,0,0);
	vec3 T2 = cross(Vh, T1);
	// Section 4.2: parameterization of the projected area
	float r = sqrt(U1);
	float phi = 2.0 * PI * U2;
	float t1 = r * cos(phi);
	float t2 = r * sin(phi);
	float s = 0.5 * (1.0 + Vh.z);
	t2 = (1.0 - s)*sqrt(1.0 - t1*t1) + s*t2;
	// Section 4.3: reprojection onto hemisphere
	vec3 Nh = t1*T1 + t2*T2 + sqrt(max(0.0, 1.0 - t1*t1 - t2*t2))*Vh;
	// Section 3.4: transforming the normal back to the ellipsoid configuration
	vec3 Ne = normalize(vec3(a * Nh.x, a * Nh.y, max(0.0, Nh.z)));

	return normalize(GetTangentSpace(n) * Ne);
}
vec3 ComputeRadianceGGX(vec3 wo, HitInfo hitInfo, out vec3 wi){
	const float specChance = 0.5;

	float a = clamp(hitInfo.roughness, 0.00001, 1.0);

	vec3 n = hitInfo.normal;
	vec3 v = -wo;

	if (Rand() <= specChance){
		vec3 m = ImportanceSampleGGXVNDF(n, v, a);

		wi = reflect(wo, m);
		vec3 h = normalize(v + wi);

		if (dot(n, wi) < 0.0) return vec3(0.0);

		vec3 F = F_schlick(v, h, mix(vec3(0.04), hitInfo.albedo, hitInfo.metalness));
		float G = G_smith(n, wi, v, a);

		return F * G / (G_GGX(n, v, a));
	}
	else{
		wi = SampleHemisphere(n);
		vec3 h = normalize(v + wi);

		vec3 lambart = hitInfo.albedo / PI;

		float pdf = 1.0 / (2.0 * PI) * (1.0 - specChance);

		vec3 kD = (1.0 - F_schlick(v, h, mix(vec3(0.04), hitInfo.albedo, hitInfo.metalness))) * (1.0 - hitInfo.metalness);

		return kD * lambart / pdf * sdot(n, wi);
	}
}
