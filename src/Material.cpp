#include "Material.h"

Material::Material() {}

Material::Material(glm::vec3 albedo, float roughness, float metalness, glm::vec3 emission) {
	this->albedo = albedo;
	this->roughness = roughness;
	this->metalness = metalness;
	this->emission = emission;
}
