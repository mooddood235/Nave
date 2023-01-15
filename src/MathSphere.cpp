#include "MathSphere.h"

MathSphere::MathSphere() : GameObject() {}

MathSphere::MathSphere(glm::vec3 position, float radius) {
	Translate(position);
	this->radius = radius;
}
float MathSphere::GetRadius() {
	return radius;
}