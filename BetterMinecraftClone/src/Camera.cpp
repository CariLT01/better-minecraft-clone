#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Camera::Camera(float ratio, float near, float far, float fov) : position(), projectionMatrix(), viewMatrix(), aspectRatio(ratio), nearPlane(near), farPlane(far), fov(fov), pitch(0.0f), yaw(-90.0f), worldUp(glm::vec3(0.0, 1.0, 0.0)) {
	initialize();
}

Camera::~Camera() {

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

glm::vec3 cardinalDirectionToVector(CardinalDirection direction) {
	switch (direction) {
	case FRONT:
		return glm::vec3(0.0f, 0.0f, 1.0f);   // Towards you
	case BACK:
		return glm::vec3(0.0f, 0.0f, -1.0f);  // Away from you
	case LEFT:
		return glm::vec3(-1.0f, 0.0f, 0.0f);  // To the left
	case RIGHT:
		return glm::vec3(1.0f, 0.0f, 0.0f);   // To the right
	case UP:
		return glm::vec3(0.0f, 1.0f, 0.0f);   // Up (Top)
	case DOWN:
		return glm::vec3(0.0f, -1.0f, 0.0f);  // Down (Bottom)
	default:
		return glm::vec3(0.0f, 0.0f, 0.0f);   // Fallback safety case
	}
}

CardinalDirection Camera::getCardinalFacingDirection() {
	const glm::vec3 standardNormals[] = {
		glm::vec3(0.0f,  0.0f,  1.0f), // 0: FRONT
		glm::vec3(0.0f,  0.0f, -1.0f), // 1: BACK
		glm::vec3(-1.0f,  0.0f,  0.0f), // 2: LEFT
		glm::vec3(1.0f,  0.0f,  0.0f), // 3: RIGHT
		glm::vec3(0.0f,  1.0f,  0.0f), // 4: TOP (UP)
		glm::vec3(0.0f, -1.0f,  0.0f)  // 5: BOTTOM (DOWN)
	};

	const CardinalDirection cardinalDirections[] = {
		FRONT,
		BACK,
		LEFT,
		RIGHT,
		UP,    // Maps to TOP
		DOWN   // Maps to BOTTOM
	};

	float maxDot = -2.0f;
	int bestIndex = 0;

	for (int i = 0; i < 6; i++) {
		float currentDot = glm::dot(front, standardNormals[i]);
		if (currentDot > maxDot) {
			maxDot = currentDot;
			bestIndex = i;
		}
	}

	return cardinalDirections[bestIndex];
}