#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include "TextureLoader.h"


TextureLoader::TextureLoader(const unsigned int blockSize) : blockSize(blockSize) {

}

AtlasResult TextureLoader::buildAtlas() {
	unsigned int channels = 4;
	unsigned int totalBlocks = textures.size();

	AtlasResult result;
	result.atlasWidth = blockSize;
	result.atlasHeight = blockSize * totalBlocks;

	result.buffer.resize(result.atlasWidth * result.atlasHeight * channels, 0);

	size_t bytesPerBlock = blockSize * blockSize * channels;
	
	for (unsigned int i = 0; i < totalBlocks; i++) {
		int imgWidth, imgHeight, channels;

		unsigned char* data = stbi_load(textures[i].c_str(), &imgWidth, &imgHeight, &channels, 4);
		if (!data) {
			std::cerr << "Failed to load: " << textures[i] << std::endl;
			continue;
		}

		if (imgWidth != blockSize || imgHeight != blockSize) {
			std::cerr << "Texture " << textures[i] << " has invalid dimensions (" << imgWidth << "x" << imgHeight << "). Expected: " << blockSize << "x" << blockSize << std::endl;
			stbi_image_free(data);
			continue;
		}

		size_t destinationOffset = i * bytesPerBlock;

		std::memcpy(result.buffer.data() + destinationOffset, data, bytesPerBlock);

		stbi_image_free(data);
	}

	return result;
}

void TextureLoader::addTexture(const std::string& texturePath) {
	std::cout << "Adding: " << texturePath << std::endl;
	textures.push_back(texturePath);
}