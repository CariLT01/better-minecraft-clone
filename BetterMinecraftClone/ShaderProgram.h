#pragma once

#include <vector>
#include "Shader.h"

class ShaderProgram {
public:
	ShaderProgram(const std::vector<Shader*> shaders);
	~ShaderProgram();

	void use();
	unsigned int getProgram();

	unsigned int getUniformLocation(const char* name);
private:

	unsigned int program;
};