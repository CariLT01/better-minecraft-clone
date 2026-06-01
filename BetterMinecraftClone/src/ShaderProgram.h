#pragma once

#include <vector>
#include "Shader.h"
#include <memory>

class ShaderProgram {
public:
	ShaderProgram(const std::vector<std::shared_ptr<Shader>> shaders);
	~ShaderProgram();

	void use();
	unsigned int getProgram();

	unsigned int getUniformLocation(const char* name);
private:

	unsigned int program;
};