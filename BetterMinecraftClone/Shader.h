#pragma once

#include <string>

class Shader {
public:
	Shader(const char* path, unsigned int type);
	~Shader();

	unsigned int getShader();
private:

	unsigned int shader;
};