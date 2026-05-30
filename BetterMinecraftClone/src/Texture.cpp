#include "Texture.h"
#include <glad/glad.h>

Texture::Texture(std::vector<unsigned char> textureData, unsigned int width, unsigned int height) {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData.data());
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::bind() {
	glBindTexture(GL_TEXTURE_2D, texture);
}

Texture::~Texture() {
	glDeleteTextures(1, &texture);
}

unsigned int Texture::getTextureID() {
	return texture;
}