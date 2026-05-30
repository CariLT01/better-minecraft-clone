#include "ShaderProgram.h"
#include <glad/glad.h>
#include <iostream>

ShaderProgram::ShaderProgram(const std::vector<Shader*> shaders) {
	program = glCreateProgram();
	for (Shader* shader : shaders) {
		glAttachShader(program, shader->getShader());
	}
	glLinkProgram(program);
	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cerr << "ERROR: Shader program linking failed: " << infoLog << std::endl;
	}
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(program);
}

void ShaderProgram::use() {
	glUseProgram(program);
}

unsigned int ShaderProgram::getProgram() {
	return program;
}

unsigned int ShaderProgram::getUniformLocation(const char* name) {
	return glGetUniformLocation(program, name);
}