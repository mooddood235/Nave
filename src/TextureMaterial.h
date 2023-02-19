#pragma once
#include <glm/glm.hpp>
#include "Texture.h"

struct TextureMaterial{
	Texture albedoTexture;
	Texture roughnessTexture;
	Texture metalnessTexture;
	Texture emissionTexture;
	Texture normalsTexture;

	glm::vec3 albedo;
	float roughness;
	float metalness;
	glm::vec3 emission;

	TextureMaterial();
	TextureMaterial(
		Texture albedoTexture,
		Texture roughnessTexture,
		Texture metalnessTexture,
		Texture emissionTexture,
		Texture normalsTexture,
		glm::vec3 albedo,
		float roughness,
		float metalness,
		glm::vec3 emission
		);
};

