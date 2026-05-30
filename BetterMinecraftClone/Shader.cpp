#include "Shader.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char* shaderSourcePath, unsigned int shaderType) {
	std::string shaderSource;
	std::ifstream shaderFile;
	// ensure ifstream objects can throw exceptions:
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		// open files
		shaderFile.open(shaderSourcePath);
		std::stringstream shaderStream;
		// read file's buffer contents into streams
		shaderStream << shaderFile.rdbuf();
		// close file handlers
		shaderFile.close();
		// convert stream into string
		shaderSource = shaderStream.str();
	}
	catch (std::ifstream::failure& e) {
		std::cerr << "ERROR: Shader file not successfully read: " << e.what() << std::endl;
	}
	const char* shaderSourceCStr = shaderSource.c_str();
	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderSourceCStr, NULL);
	glCompileShader(shader);
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cerr << "ERROR: Shader compilation failed: " << infoLog << std::endl;
	}
}

unsigned int Shader::getShader() {
	return shader;
}

Shader::~Shader() {
	glDeleteShader(shader);
}