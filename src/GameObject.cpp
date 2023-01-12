#include "GameObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

void GameObject::Translate(glm::vec3 translation) {
	translationMatrix = glm::translate(translationMatrix, translation);
}
void GameObject::Scale(glm::vec3 scale) {
	scaleMatrix = glm::scale(scaleMatrix, scale);
}
void GameObject::Rotate(float angleInDegrees, glm::vec3 axis, Space space) {
	if (space == Space::global) {
		axis = glm::inverse(glm::mat3(rotationMatrix)) * axis;
	}
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(angleInDegrees), axis);
}