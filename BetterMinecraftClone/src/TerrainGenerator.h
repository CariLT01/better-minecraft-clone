#include "Chunk.h"
#include "FastNoiseLite.h"

class TerrainGenerator {
public:
	TerrainGenerator();

	Chunk* generateChunk(int x, int y, int z);
private:
	FastNoiseLite* noise;
};