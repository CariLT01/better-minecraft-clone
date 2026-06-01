#pragma once

#include "ShaderProgram.h"
#include <vector>
#include <glm/glm.hpp>
#include "Camera.h"
#include "TextureArray.h"
#include "config.h"
#include <memory>

#pragma pack(push, 1)
struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	int packedData;
};
#pragma pack(pop)

struct RenderingContext {
	std::shared_ptr < ShaderProgram> shaderProgram;
	std::shared_ptr < TextureArray> textureAtlas;
	unsigned int numTextures;
};

class ChunkMesh {
public:
	ChunkMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const RenderingContext& context, glm::vec3 offset);
	~ChunkMesh();


	// Don't copy
	ChunkMesh(const ChunkMesh&) = delete;
	ChunkMesh& operator=(const ChunkMesh&) = delete;

	void render(std::shared_ptr < Camera> camera);

private:

	void createMesh();

	unsigned int vbo;
	unsigned int vao;
	unsigned int size;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	RenderingContext context;

	glm::vec3 offset;
};