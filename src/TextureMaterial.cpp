#include "TextureMaterial.h"

TextureMaterial::TextureMaterial() {
	albedoTexture = Texture();
	roughnessTexture = Texture();
	metalnessTexture = Texture();
	emissionTexture = Texture();
	normalsTexture = Texture();
}

TextureMaterial::TextureMaterial(
	Texture albedoTexture,
	Texture roughnessTexture,
	Texture metalnessTexture,
	Texture emissionTexture,
	Texture normalsTexture,
	glm::vec3 albedo,
	float roughness,
	float metalness,
	glm::vec3 emission
	) {
	this->albedoTexture = albedoTexture;
	this->roughnessTexture = roughnessTexture;
	this->metalnessTexture = metalnessTexture;
	this->emissionTexture = emissionTexture;
	this->normalsTexture = normalsTexture;
	this->albedo = albedo;
	this->roughness = roughness;
	this->metalness = metalness;
	this->emission = emission;
}