#pragma once

#include "config.h"
#include <glm/glm.hpp>

enum CardinalDirection {
	UP,
	DOWN,
	FRONT,
	BACK,
	LEFT,
	RIGHT
};

glm::vec3 cardinalDirectionToVector(CardinalDirection direction);

class Camera {
public:
	Camera(float aspectRatio, float near, float far, float fov);
	~Camera();

	void update(float mousexpos, float mouseypos);

	glm::mat4 getView();
	glm::mat4 getProjection();
	glm::vec3 getPosition() {
		return position;
	}
	glm::vec3 getFront() {
		return front;
	}


	void moveLocal(const glm::vec3& localOffset);

	CardinalDirection getCardinalFacingDirection();

private:
	glm::vec3 position;
	float pitch;
	float yaw;

	// values
	float aspectRatio;
	float nearPlane;
	float farPlane;
	float fov;
	
	// direction vectors
	glm::vec3 up;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 worldUp;

	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;

	void initialize();

	void updateVectors();
	void updateMatrices();

	float lastMouseX = 0.0f;
	float lastMouseY = 0.0f;

	bool firstFrame = true;

};