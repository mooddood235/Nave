#pragma once
#include <glm/glm.hpp>

struct Material{
	glm::vec3 albedo = glm::vec3(1.0f);
	float roughness = 1.0f;
	float metalness = 0.0f;

	Material();
	Material(glm::vec3 albedo, float roughness, float metalness);
};

