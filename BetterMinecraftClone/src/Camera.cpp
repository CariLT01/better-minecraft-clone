#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Camera::Camera(float ratio, float near, float far, float fov) : position(), projectionMatrix(), viewMatrix(), aspectRatio(ratio), nearPlane(near), farPlane(far), fov(fov), pitch(0.0f), yaw(-90.0f), worldUp(glm::vec3(0.0, 1.0, 0.0)) {
	initialize();
}

void Camera::initialize() {

}

void Camera::update(float mousexpos, float mouseypos) {

	// std::cout << "Mouse X: " << mousexpos << " Mouse Y: " << mouseypos << std::endl;

	if (firstFrame) {
		lastMouseX = mousexpos;
		lastMouseY = mouseypos;
		firstFrame = false;
		return;
	}

	float dx = mousexpos - lastMouseX;
	float dy = mouseypos - lastMouseY;

	lastMouseX = mousexpos;
	lastMouseY = mouseypos;

	// std::cout << "Mouse X: " << mousexpos << " dx: " << dx << " dy: " << dy << std::endl;

	yaw += dx * 0.05f;
	pitch -= dy * 0.05f;
	if (pitch > 89.0f)  pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	updateVectors();
	updateMatrices();

	// std::cout << "Camera is at: " << position.x << ", " << position.y << ", " << position.z << " pitch: " << pitch << " yaw: " << yaw << std::endl;
}

void Camera::updateVectors() {
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}

void Camera::updateMatrices() {
	projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
	viewMatrix = glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getView() {
	return viewMatrix;
}

glm::mat4 Camera::getProjection() {
	return projectionMatrix;
}

void Camera::moveLocal(const glm::vec3& localOffset) {
	// Calculate the world-space displacement
	glm::vec3 worldOffset = (right * localOffset.x) +
		(up * localOffset.y) +
		(front * localOffset.z);

	// Apply it to the current position
	position += worldOffset;
}