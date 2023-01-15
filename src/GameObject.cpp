#include "GameObject.h"
#include <glm/gtc/matrix_transform.hpp>

void GameObject::Translate(glm::vec3 translation, Space space) {
	if (space == Space::local) translation = glm::mat3(rotationMatrix) * translation;
	position += translation;
	translationMatrix = glm::translate(translationMatrix, translation);
}
void GameObject::Scale(glm::vec3 scale, Space space) {
	if (space == Space::local) scale = glm::mat3(scaleMatrix) * scale;
	this->scale += scale;
	scaleMatrix = glm::scale(scaleMatrix, scale);
}
void GameObject::Rotate(float angleInDegrees, glm::vec3 axis, Space space) {
	if (space == Space::global) axis = glm::normalize(glm::inverse(glm::mat3(rotationMatrix)) * axis);
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(angleInDegrees), axis);
}
glm::mat4 GameObject::GetModelMatrix() {
	return translationMatrix * scaleMatrix * rotationMatrix;
}
glm::vec3 GameObject::GetPosition() {
	return position;
}
glm::vec3 GameObject::GetScale() {
	return scale;
}
glm::vec3 GameObject::GetXAxis() {
	return glm::normalize(glm::mat3(rotationMatrix) * glm::vec3(1, 0, 0));
}
glm::vec3 GameObject::GetYAxis() {
	return glm::normalize(glm::mat3(rotationMatrix) * glm::vec3(0, 1, 0));
}
glm::vec3 GameObject::GetZAxis() {
	return glm::normalize(glm::mat3(rotationMatrix) * glm::vec3(0, 0, 1));
}