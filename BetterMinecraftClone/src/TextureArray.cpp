
#include "stb_image.h"
#include "TextureArray.h"
#include <glad/glad.h>
#include "config.h"
#include <iostream>

TextureArray::TextureArray() : textures(), texture(0) {

}

void TextureArray::create() {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLsizei width = TEXTURE_W;
	GLsizei height = TEXTURE_H;
	GLsizei layers = textures.size();

	GLsizei levels = std::floor(std::log2(std::max(width, height))) + 1;

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, levels, GL_RGBA8, width, height, layers);
	
	unsigned int i = 0;
	for (std::vector<unsigned char> imageData : textures) {
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
			0, 0, i,
			width, height, 1,
			GL_RGBA, GL_UNSIGNED_BYTE, imageData.data());
		i++;
	}

	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

}

void TextureArray::addTexture(std::vector<unsigned char> textureData) {
	textures.push_back(textureData);
}

std::vector<unsigned char> TextureArray::loadTexture(const std::string& path) {
	int w;
	int h;
	unsigned char* imageData = stbi_load(path.c_str(), &w, &h, NULL, 4);
	if (!imageData) {
		std::cerr << "Failed to load  image: " << path << std::endl;
		return {};
	}
	if (w != TEXTURE_W || h != TEXTURE_H) {
		std::cerr << "Image specifications do not match what was defined: Got: " << w << "x" << h << " expected: " << TEXTURE_W << "x" << TEXTURE_H << std::endl;
		return {};
	}

	std::vector<unsigned char> imageDataVec = std::vector<unsigned char>(imageData, imageData + (w * h * 4));

	std::cout << "Texture loaded: " << path << std::endl;

	return imageDataVec;

}

void TextureArray::bind() {
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
}