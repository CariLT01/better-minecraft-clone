#include <vector>

class Texture {
public:
	Texture(std::vector<unsigned char> data, unsigned int width, unsigned int height);
	~Texture();

	unsigned int getTextureID();
	
	void bind();

private:

	unsigned int texture;
};