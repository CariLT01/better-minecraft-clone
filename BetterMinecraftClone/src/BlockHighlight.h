#pragma once

#include "ShaderProgram.h"
#include "WorldChunks.h"
#include "Camera.h"

constexpr unsigned int verticesCount = 12 * 2 * 3;
constexpr float vertices[verticesCount] = {
    // --- Bottom Ring (Y = -1.0) ---
    -1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f, // Back edge
     1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  1.0f, // Right edge
     1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f, // Front edge
    -1.0f, -1.0f,  1.0f,  -1.0f, -1.0f, -1.0f, // Left edge

    // --- Top Ring (Y = 1.0) ---
    -1.0f,  1.0f, -1.0f,   1.0f,  1.0f, -1.0f, // Back edge
     1.0f,  1.0f, -1.0f,   1.0f,  1.0f,  1.0f, // Right edge
     1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f, // Front edge
    -1.0f,  1.0f,  1.0f,  -1.0f,  1.0f, -1.0f, // Left edge

    // --- Vertical Pillars (Connecting Bottom to Top) ---
    -1.0f, -1.0f, -1.0f,  -1.0f,  1.0f, -1.0f, // Back-Left pillar
     1.0f, -1.0f, -1.0f,   1.0f,  1.0f, -1.0f, // Back-Right pillar
     1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f, // Front-Right pillar
    -1.0f, -1.0f,  1.0f,  -1.0f,  1.0f,  1.0f  // Front-Left pillar
};


class BlockHighlight {
public:
	BlockHighlight();
	~BlockHighlight();

	void initialize();
	void render(std::shared_ptr<Camera> camera);

	void update(std::shared_ptr<Camera> camera, std::shared_ptr<WorldChunks> chunks);

    glm::vec3 getHitPos() {
        return offset;
    }

    glm::vec3 getHitNormal() {
        return hitNormal;
    }

    bool getHit() {
        return hit;
    }

    glm::vec3 getPlacementOffset(Camera* camera);

private:
	unsigned int vbo;
	unsigned int vao;

	std::shared_ptr<ShaderProgram> shaderProgram;

	glm::vec3 offset;
    glm::vec3 hitNormal;

	void initializeShaders();
	void initializeMesh();

    bool hit;
};