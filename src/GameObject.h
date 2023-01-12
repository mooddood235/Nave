#pragma once
#include <glm/glm.hpp>

enum class Space {
	local,
	global
};

class GameObject{
public:
	void Translate(glm::vec3 translation);
	void Scale(glm::vec3 scale);
	void Rotate(float angleInDegrees, glm::vec3 axis, Space space = Space::global);
private:
	glm::mat4 translationMatrix = glm::mat4(1);
	glm::mat4 scaleMatrix = glm::mat4(1);
	glm::mat4 rotationMatrix = glm::mat4(1);
};



