#pragma once
#include <glm/glm.hpp>

struct AABB {
	glm::vec3 cornerMin;
	glm::vec3 cornerMax;
	AABB();
	AABB(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);
	AABB(AABB aabb0, AABB aabb1);
	AABB(glm::vec3 cornerMin, glm::vec3 cornerMax);
	glm::vec3 GetCentroid();
	float GetSurfaceArea();
};

