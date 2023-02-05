#include "MathSphere.h"

MathSphere::MathSphere() : GameObject() {}

MathSphere::MathSphere(glm::vec3 position, float radius, Material material) {
	Translate(position);
	this->radius = radius;
	this->material = material;
}
MathSphere::MathSphere(Material material) {
	this->material = material;
}
float MathSphere::GetRadius() {
	return radius;
}