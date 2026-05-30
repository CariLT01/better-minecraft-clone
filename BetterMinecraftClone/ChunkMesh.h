#pragma once

#include "ShaderProgram.h"
#include <vector>
#include <glm/glm.hpp>
#include "Camera.h"

#pragma pack(push, 1)
struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
};
#pragma pack(pop)

class ChunkMesh {
public:
	ChunkMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, ShaderProgram* program, glm::vec3 offset);
	~ChunkMesh();

	void render(Camera* camera);

private:

	void createMesh();

	unsigned int vbo;
	unsigned int vao;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	ShaderProgram* program;

	glm::vec3 offset;
};