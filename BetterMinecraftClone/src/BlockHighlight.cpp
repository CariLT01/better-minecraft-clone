#include "BlockHighlight.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

BlockHighlight::BlockHighlight() : hit(false), hitNormal(glm::vec3(0.0f)), offset(glm::vec3(0.0f)), vao(0), vbo(0) {

}

BlockHighlight::~BlockHighlight() {
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void BlockHighlight::initializeShaders() {
	std::shared_ptr<Shader> vertexShader = std::make_shared<Shader>("shaders/block_highlight.vert", GL_VERTEX_SHADER);
	std::shared_ptr<Shader> fragmentShader = std::make_shared<Shader>("shaders/block_highlight.frag", GL_FRAGMENT_SHADER);

	shaderProgram = std::make_shared<ShaderProgram>(std::vector<std::shared_ptr<Shader>>{ vertexShader, fragmentShader });

}

void BlockHighlight::initializeMesh() {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, verticesCount * sizeof(float), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

}

void BlockHighlight::initialize() {
	initializeShaders();
	initializeMesh();
}

void BlockHighlight::update(std::shared_ptr<Camera> camera, std::shared_ptr<WorldChunks> chunks) {
	// TODO: replace with DDA algorithm

	float stepSize = 0.05f;
	float d = 0.0f;

	glm::vec3 pos = camera->getPosition();
	glm::vec3 dir = camera->getFront();

	float x = pos.x;
	float y = pos.y;
	float z = pos.z;

	float dx = dir.x * stepSize;
	float dy = dir.y * stepSize;
	float dz = dir.z * stepSize;

	int lastX = floor(x);
	int lastY = floor(y);
	int lastZ = floor(z);

	hit = false;

	while (d < 5.0f) {
		x += dx;
		y += dy;
		z += dz;
		d += stepSize;


		int fx = floor(x);
		int fy = floor(y);
		int fz = floor(z);

		if (chunks->getBlockAt(fx, fy, fz) != 0) {
			hit = true;

			if (fx != lastX) {
				hitNormal = glm::vec3(lastX - fx, 0.0f, 0.0f); // Returns (-1, 0, 0) or (1, 0, 0)
			}
			else if (fy != lastY) {
				hitNormal = glm::vec3(0.0f, lastY - fy, 0.0f); // Returns (0, -1, 0) or (0, 1, 0)
			}
			else if (fz != lastZ) {
				hitNormal = glm::vec3(0.0f, 0.0f, lastZ - fz); // Returns (0, 0, -1) or (0, 0, 1)
			}

			break;
		}

		lastX = fx;
		lastY = fy;
		lastZ = fz;
	}

	int bx = floor(x);
	int by = floor(y);
	int bz = floor(z);

	offset.x = bx;
	offset.y = by;
	offset.z = bz;

}

void BlockHighlight::render(std::shared_ptr < Camera> camera) {

	if (!hit) return;

	shaderProgram->use();
	unsigned int offsetLoc = shaderProgram->getUniformLocation("uOffset");
	glUniform3fv(offsetLoc, 1, glm::value_ptr(offset));

	unsigned int viewLoc = shaderProgram->getUniformLocation("uView");
	unsigned int projLoc = shaderProgram->getUniformLocation("uProjection");

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->getView()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(camera->getProjection()));

	glBindVertexArray(vao);
	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 0, verticesCount);
	glLineWidth(1.0f);
}

glm::vec3 BlockHighlight::getPlacementOffset(Camera* camera) {
	CardinalDirection facingDir = camera->getCardinalFacingDirection();
	
	return cardinalDirectionToVector(facingDir) * glm::vec3(-1.0f);
}