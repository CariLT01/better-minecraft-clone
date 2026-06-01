#pragma once

#include <vector>
#include <string>

class TextureArray {
public:
	TextureArray();
	~TextureArray();
	
	void addTexture(std::vector<unsigned char> textureData);
	std::vector<unsigned char> loadTexture(const std::string& path);

	void bind();
	void create();

private:
	std::vector<std::vector<unsigned char>> textures;

	unsigned int texture;
};