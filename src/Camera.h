#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GameObject.h"

class Camera : public GameObject{
public:
	Camera(float yFOVInDegrees, float nearClip, float farClip);
	Camera(glm::vec3 position, float yFOVInDegrees, float nearClip, float farClip);
	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetRotationMatrix();
	void ProcessInput();
	void SetYFOV(float yFOVinDegrees);
	float GetFocalLength();

private:
	float yFOVInDegrees;
	float focalLength;
	float nearClip;
	float farClip;

	const float moveSpeed = 15.0f;
	const float lookSensitivity = 0.05f;

	bool firstMouse = false;

	float lastX = 0;
	float lastY = 0;
};

