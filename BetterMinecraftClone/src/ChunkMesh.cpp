#include "ChunkMesh.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

ChunkMesh::ChunkMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, const RenderingContext& context, glm::vec3 offset) :
vertices(vertices), indices(indices), context(context), offset(offset) {
	createMesh();
}

ChunkMesh::~ChunkMesh() {
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &vao);
}

void ChunkMesh::createMesh() {

	// --- VAO setup
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// --- VBO setup
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	// bind layout
	// 3 floats for position

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position) + sizeof(glm::vec3)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position) + sizeof(glm::vec3) + sizeof(glm::vec3)));
	glVertexAttribIPointer(3, 1, GL_INT, sizeof(Vertex), (void*)(offsetof(Vertex, position) + sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

}

void ChunkMesh::render(Camera* camera) {
	context.shaderProgram->use();
	glActiveTexture(GL_TEXTURE0);
	context.textureAtlas->bind();

	unsigned int projLoc = context.shaderProgram->getUniformLocation("uProjection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(camera->getProjection()));
	unsigned int viewLoc = context.shaderProgram->getUniformLocation("uView");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->getView()));
	unsigned int offsetLoc = context.shaderProgram->getUniformLocation("uOffset");
	glUniform3fv(offsetLoc, 1, glm::value_ptr(offset));
	unsigned int texLoc = context.shaderProgram->getUniformLocation("uTextureAtlas");
	glUniform1i(texLoc, 0);
	unsigned int totalTexturesLoc = context.shaderProgram->getUniformLocation("uTotalTextures");
	glUniform1i(totalTexturesLoc, context.numTextures);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

