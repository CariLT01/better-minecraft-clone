#include "ChunkMesh.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

ChunkMesh::ChunkMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, ShaderProgram* program, glm::vec3 offset) :
vertices(vertices), indices(indices), program(program), offset(offset) {
	createMesh();
}

ChunkMesh::~ChunkMesh() {
	glDeleteBuffers(1, &vbo);
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
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

}

void ChunkMesh::render(Camera* camera) {
	program->use();

	unsigned int projLoc = program->getUniformLocation("uProjection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(camera->getProjection()));
	unsigned int viewLoc = program->getUniformLocation("uView");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->getView()));
	unsigned int offsetLoc = program->getUniformLocation("uOffset");
	glUniform3fv(offsetLoc, 1, glm::value_ptr(offset));

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

