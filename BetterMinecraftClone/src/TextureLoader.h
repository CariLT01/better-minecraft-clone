#include <vector>

struct AtlasResult {
	std::vector<unsigned char> buffer;
	int atlasWidth;
	int atlasHeight;
};

class TextureLoader {
public:
	TextureLoader(const unsigned int blockSize);

	void addTexture(const std::string& texturePath);

	AtlasResult buildAtlas();

private:

	std::vector<std::string> textures;
	unsigned int blockSize;
};