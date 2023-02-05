#include "Camera.h"
#include <iostream>
#include <string>
#include "WindowInfo.h"

Camera::Camera(float yFOVInDegrees, float nearClip, float farClip) {
	this->yFOVInDegrees = yFOVInDegrees;
	this->nearClip = nearClip;
	this->farClip = farClip;

	focalLength = (float)WINDOWHEIGHT / (2.0f * glm::tan(yFOVInDegrees / 2));
}
Camera::Camera(glm::vec3 position, float yFOVInDegrees, float nearClip, float farClip) {
	Translate(position);
	Camera::Camera(yFOVInDegrees, nearClip, farClip);
}
void Camera::ProcessInput() {
	// Process move
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) Translate(glm::vec3(0, 0, -1) * moveSpeed * deltaTime, Space::local);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) Translate(glm::vec3(0, 0, 1) * moveSpeed * deltaTime, Space::local);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) Translate(glm::vec3(-1, 0, 0) * moveSpeed * deltaTime, Space::local);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) Translate(glm::vec3(1, 0, 0) * moveSpeed * deltaTime, Space::local);

	// Process look
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	if (!firstMouse) {
		lastX = mouseX;
		lastY = mouseY;
		firstMouse = true;
	}

	double xOffset = mouseX - lastX;
	double yOffset = mouseY - lastY;

	Rotate(-xOffset * lookSensitivity, glm::vec3(0, 1, 0));
	Rotate(-yOffset * lookSensitivity, glm::vec3(1, 0, 0), Space::local);

	lastX = mouseX;
	lastY = mouseY;
}
void Camera::SetYFOV(float yFOVInDegrees) {
	this->yFOVInDegrees = yFOVInDegrees;
}
float Camera::GetFocalLength() {
	return focalLength;
}
glm::mat4 Camera::GetViewMatrix() {
	return glm::inverse(GetModelMatrix());
}
glm::mat4 Camera::GetProjectionMatrix() {
	return glm::perspective(glm::radians(yFOVInDegrees), (float)WINDOWWIDTH / (float)WINDOWHEIGHT, nearClip, farClip);
}
glm::mat4 Camera::GetRotationMatrix() {
	return rotationMatrix;
}