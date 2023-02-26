#version 460 core
#extension ARB_gpu_shader_int64: enable

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

	vec3 emission;
};

struct Vertex{
	vec3 position;
	float pad0;
	vec3 normal;
	float pad1;
	vec3 tangent;
	float pad2;
	vec3 biTangent;
	float pad3;
	vec2 uv;
	uint64_t pad4;
};
struct BVHNode{
	uint isLeaf;
	uint left;
	uint right;
	float pad2;

	vec3 cornerMin;
	float pad0;
	vec3 cornerMax;
	float pad1;
};

struct MathSphere{
	vec3 position;
	float radius;
	vec3 albedo;
	float roughness;
	float metalness;
	vec3 emission;
};

struct TextureMaterial{
	uint64_t albedoTexture;
	uint64_t roughnessTexture;
	uint64_t metalnessTexture;
	uint64_t emissionTexture;
	uint64_t normalTexture;
	uint64_t pad0;

	vec3 albedo;
	float pad1;
	float roughness;
	float metalness;
	uint64_t pad2;
	vec3 emission;
	float pad3;
};


// Constants
const float PI = 3.14159265359;
const HitInfo NoHit = HitInfo(false, 1. / 0., vec3(0), vec3(0), vec3(0), 0, 0, vec3(0));

// Helper functions
float Rand();
vec3 At(Ray ray, float t);
vec3 SampleEnvironmentMap(vec3 direction);
vec3 SampleHemisphere(vec3 n);
vec3 CosineSampleHemisphere(vec3 n);
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
HitInfo Hit_Triangle(Vertex v0, Vertex v1, Vertex v2, Ray ray, uint textureMaterialIndex);
HitInfo Hit_AABB(vec3 cornerMin, vec3 cornerMax, Ray ray);

in vec2 uv;

layout (location = 0) out vec3 CumulativeColor;
layout (location = 1) out vec3 FinalColor;

uniform sampler2D cumulativeRenderTexture;

uniform uint currSample;

uniform uint seed;
uint _seed;

uniform sampler2D environmentMap;

uniform Camera camera;
uniform uint maxDepth;

layout (std140) buffer Vertices{
	Vertex vertices[];
};
layout (std140) buffer Indices{
	uint indices[];
};
layout (std140) buffer BVH{
	BVHNode bvh[];
};
layout (std140) buffer TextureMaterials{
	TextureMaterial textureMaterials[];
};

uniform uint indexCount;
uniform uint nodeCount;

void main(){
	_seed = seed;

	vec2 multisampleUV = vec2(uv.x + (Rand() * 2.0 - 1.0) / camera.viewPortWidth, uv.y + (Rand() * 2.0 - 1.0) / camera.viewPortHeight);

	vec3 worldUV = camera.position + 
	-camera.zAxis * camera.focalLength + 
	camera.xAxis * float(camera.viewPortWidth) / 2.0 * multisampleUV.x +
	camera.yAxis * float(camera.viewPortHeight) / 2.0 * multisampleUV.y;

	Ray ray = Ray(camera.position, normalize(worldUV - camera.position));

	vec3 radiance = vec3(1.0);

	uint stack[20];

	for (uint depth = 1; depth <= maxDepth + 1; depth++){
		if (depth == maxDepth + 1){
			radiance = vec3(0.0);
			break;
		}
		HitInfo closestHit = NoHit;

		uint stackSize = 1;
		stack[0] = 0;

		while (stackSize > 0){
			BVHNode node = bvh[stack[stackSize - 1]];
			stackSize--;

			if (node.isLeaf == 0){
				BVHNode leftNode = bvh[node.left];
				BVHNode rightNode = bvh[node.right];

				HitInfo leftHitInfo = Hit_AABB(leftNode.cornerMin, leftNode.cornerMax, ray);
				HitInfo rightHitInfo = Hit_AABB(rightNode.cornerMin, rightNode.cornerMax, ray);

				bool pushLeft = leftHitInfo.didHit && leftHitInfo.t < closestHit.t;
				bool pushRight = rightHitInfo.didHit && rightHitInfo.t < closestHit.t;

				if (pushLeft){
					stack[stackSize] = node.left;
					stackSize++;
				}
				if (pushRight){
					stack[stackSize] = node.right;
					stackSize++;			
				}
				if (pushLeft && pushRight && leftHitInfo.t < rightHitInfo.t){
					stack[stackSize - 1] = node.left;
					stack[stackSize - 2] = node.right;
				}
			}
			else{
				Vertex v0 = vertices[indices[node.left]];
				Vertex v1 = vertices[indices[node.left + 1]];
				Vertex v2 = vertices[indices[node.left + 2]];

				HitInfo hitInfo = Hit_Triangle(v0, v1, v2, ray, node.right);
				if (hitInfo.didHit && hitInfo.t < closestHit.t) closestHit = hitInfo;
			}
		}
		if (closestHit.didHit){
			vec3 wi;

			if (closestHit.emission != vec3(0)) {
				radiance *= closestHit.emission;
				break;
			}
			radiance *= ComputeRadianceGGX(ray.direction, closestHit, wi);

			ray.origin = closestHit.position + closestHit.normal * 0.0001;
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
HitInfo Hit_Triangle(Vertex v0, Vertex v1, Vertex v2, Ray ray, uint textureMaterialIndex){
	const float EPSILON = 0.0000001;
    vec3 vertex0 = v0.position;
    vec3 vertex1 = v1.position;  
    vec3 vertex2 = v2.position;
    vec3 edge1, edge2, h, s, q;
    float a,f,u,v;
    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;
    h = cross(ray.direction, edge2);
    a = dot(edge1, h);
    if (a > -EPSILON && a < EPSILON)
        return NoHit;    // This ray is parallel to this triangle.
    f = 1.0/a;
    s = ray.origin - vertex0;
    u = f * dot(s, h);
    if (u < 0.0 || u > 1.0)
        return NoHit;
    q = cross(s, edge1);
    v = f * dot(ray.direction, q);
    if (v < 0.0 || u + v > 1.0)
        return NoHit;
    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * dot(edge2, q);

	if (a > -EPSILON && a < EPSILON || u < 0.0 || u > 1.0 || t <= EPSILON) return NoHit;

	float w = 1.0 - u - v;

	vec2 uv = v0.uv * w + v1.uv * u + v2.uv * v;

	vec3 trueNormal = normalize(v0.normal * w + v1.normal * u + v2.normal * v);

	mat3 TBN = mat3(
		normalize(v0.tangent * w + v1.tangent * u + v2.tangent * v),
		normalize(v0.biTangent * w + v1.biTangent * u + v2.biTangent * v),
		trueNormal
	);

	vec3 albedoProperty = textureMaterials[textureMaterialIndex].albedo;
	float roughnessProperty = textureMaterials[textureMaterialIndex].roughness;
	float metalnessProperty = textureMaterials[textureMaterialIndex].metalness;
	vec3 emissionProperty = textureMaterials[textureMaterialIndex].emission;
	
	float mipMapLevel = 0;

	vec3 albedo = textureLod(sampler2D(textureMaterials[textureMaterialIndex].albedoTexture), uv, mipMapLevel).rgb + albedoProperty;
	float roughness = textureLod(sampler2D(textureMaterials[textureMaterialIndex].roughnessTexture), uv, mipMapLevel).r + roughnessProperty;
	float metalness = textureLod(sampler2D(textureMaterials[textureMaterialIndex].metalnessTexture), uv, mipMapLevel).r + metalnessProperty;
	vec3 emission = textureLod(sampler2D(textureMaterials[textureMaterialIndex].emissionTexture), uv, mipMapLevel).rgb + emissionProperty;
	vec3 textureNormal = textureLod(sampler2D(textureMaterials[textureMaterialIndex].normalTexture), uv, mipMapLevel).rgb * 2.0 - 1.0;

	vec3 normal = normalize(TBN * textureNormal);

    return HitInfo(true, t, At(ray, t), normal, albedo, roughness, metalness, emission);
}
    

 HitInfo Hit_AABB(vec3 cornerMin, vec3 cornerMax, Ray ray){
	float tx1 = (cornerMin.x - ray.origin.x) / ray.direction.x, tx2 = (cornerMax.x - ray.origin.x) / ray.direction.x;
    float tmin = min( tx1, tx2 ), tmax = max( tx1, tx2 );
    float ty1 = (cornerMin.y - ray.origin.y) / ray.direction.y, ty2 = (cornerMax.y - ray.origin.y) / ray.direction.y;
    tmin = max( tmin, min( ty1, ty2 ) ), tmax = min( tmax, max( ty1, ty2 ) );
    float tz1 = (cornerMin.z - ray.origin.z) / ray.direction.z, tz2 = (cornerMax.z - ray.origin.z) / ray.direction.z;
    tmin = max( tmin, min( tz1, tz2 ) ), tmax = min( tmax, max( tz1, tz2 ) );
    if (tmax >= tmin && tmax > 0){
		HitInfo hitInfo;
		hitInfo.didHit = true;
		hitInfo.t = clamp(tmin, 0, tmin);
		return hitInfo;
	}
	return NoHit;
}
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

vec3 SampleHemisphere(vec3 n){
	// Uniformly sample hemisphere direction
    float cosTheta = Rand();
    float sinTheta = sqrt(max(0.0, 1.0 - cosTheta * cosTheta));
    float phi = 2 * PI * Rand();
    vec3 sampleVector = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
    // Transform direction to world space
    return normalize(GetTangentSpace(n) * sampleVector);
};
vec3 CosineSampleHemisphere(vec3 n){
	float theta = acos(sqrt(Rand()));
	float phi = 2.0 * PI * Rand();

	return normalize(GetTangentSpace(n) * vec3(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta)));
}

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

	float a = clamp(hitInfo.roughness * hitInfo.roughness, 0.0, 1.0);

	vec3 n = hitInfo.normal;
	vec3 v = -wo;

	if (Rand() <= specChance){
		vec3 m = ImportanceSampleGGXVNDF(n, v, a);

		wi = reflect(wo, m);
		vec3 h = normalize(v + wi);

		if (dot(n, wi) < 0.0) return vec3(0.0);

		vec3 F = F_schlick(v, h, mix(vec3(0.04), hitInfo.albedo, hitInfo.metalness));
		float G = G_smith(n, wi, v, a);
		
		return F * G / (G_GGX(n, v, a) + 0.00001);
	}
	else{
		wi = CosineSampleHemisphere(n);
		vec3 h = normalize(v + wi);

		vec3 lambart = hitInfo.albedo / PI;

		float pdf = (sdot(n, wi) / PI) * (1.0 - specChance);

		vec3 kD = (1.0 - F_schlick(v, h, mix(vec3(0.04), hitInfo.albedo, hitInfo.metalness))) * (1.0 - hitInfo.metalness);

		return kD * lambart / pdf * sdot(n, wi);
	}
}
