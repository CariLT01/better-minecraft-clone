#pragma once

#include "ShaderProgram.h"
#include <vector>
#include <glm/glm.hpp>
#include "Camera.h"
#include "TextureArray.h"

#pragma pack(push, 1)
struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	int textureType;
};
#pragma pack(pop)

struct RenderingContext {
	ShaderProgram* shaderProgram;
	TextureArray* textureAtlas;
	unsigned int numTextures;
};

class ChunkMesh {
public:
	ChunkMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, const RenderingContext& context, glm::vec3 offset);
	~ChunkMesh();

	void render(Camera* camera);

private:

	void createMesh();

	unsigned int vbo;
	unsigned int vao;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	RenderingContext context;

	glm::vec3 offset;
};