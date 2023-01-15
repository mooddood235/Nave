#pragma once
#include <glm/glm.hpp>

enum class Space {
	local,
	global
};

class GameObject{
public:
	void Translate(glm::vec3 translation, Space space = Space::global);
	void Scale(glm::vec3 scale, Space space = Space::global);
	void Rotate(float angleInDegrees, glm::vec3 axis, Space space = Space::global);
	glm::mat4 GetModelMatrix();
	glm::vec3 GetPosition();
	glm::vec3 GetScale();
	glm::vec3 GetXAxis();
	glm::vec3 GetYAxis();
	glm::vec3 GetZAxis();

protected:
	glm::mat4 translationMatrix = glm::mat4(1);
	glm::mat4 scaleMatrix = glm::mat4(1);
	glm::mat4 rotationMatrix = glm::mat4(1);

	glm::vec3 position = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);
};



