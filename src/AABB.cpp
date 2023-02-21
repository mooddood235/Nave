#include "AABB.h"

AABB::AABB() {
	cornerMin = glm::vec3(0);
	cornerMax = glm::vec3(0);
}

AABB::AABB(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) {
	cornerMin = glm::vec3(
		glm::min(glm::min(v0.x, v1.x), v2.x),
		glm::min(glm::min(v0.y, v1.y), v2.y),
		glm::min(glm::min(v0.z, v1.z), v2.z)
	);
	cornerMax = glm::vec3(
		glm::max(glm::max(v0.x, v1.x), v2.x),
		glm::max(glm::max(v0.y, v1.y), v2.y),
		glm::max(glm::max(v0.z, v1.z), v2.z)
	);
}
AABB::AABB(AABB aabb0, AABB aabb1) {
	cornerMin = glm::vec3(
		glm::min(aabb0.cornerMin.x, aabb1.cornerMin.x),
		glm::min(aabb0.cornerMin.y, aabb1.cornerMin.y),
		glm::min(aabb0.cornerMin.z, aabb1.cornerMin.z)
	);
	cornerMax = glm::vec3(
		glm::max(aabb0.cornerMax.x, aabb1.cornerMax.x),
		glm::max(aabb0.cornerMax.y, aabb1.cornerMax.y),
		glm::max(aabb0.cornerMax.z, aabb1.cornerMax.z)
	);
}
glm::vec3 AABB::GetCentroid() {
	return (cornerMin + cornerMax) / 2.0f;
}
float AABB::GetSurfaceArea() {
	float width = cornerMax.x - cornerMin.x;
	float height = cornerMax.y - cornerMin.y;
	float depth = cornerMax.z - cornerMin.z;

	return 2.0f * width * depth + 2.0f * width * height + 2.0f * depth * height;
}