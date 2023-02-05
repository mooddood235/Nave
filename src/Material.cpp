#include "Material.h"

Material::Material() {}

Material::Material(glm::vec3 albedo, float roughness, float metalness) {
	this->albedo = albedo;
	this->roughness = roughness;
	this->metalness = metalness;
}
