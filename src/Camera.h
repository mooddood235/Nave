#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include "GameObject.h"

class Camera : public GameObject{
public:
	Camera(float yFOVInDegrees, float nearClip, float farClip);
	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();
	void ProcessInput();
private:
	float yFOVInDegrees;
	float nearClip;
	float farClip;

	const float moveSpeed = 15.0f;
	const float lookSensitivity = 0.05f;

	bool firstMouse = false;

	float lastX = 0;
	float lastY = 0;
};

