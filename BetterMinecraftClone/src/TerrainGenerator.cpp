#include "TerrainGenerator.h"
#include "MathUtils.h"
#include <cmath>

TerrainGenerator::TerrainGenerator() {

}

Chunk* TerrainGenerator::generateChunk(int cx, int cy, int cz) {

	float gridArray[CHUNK_VOLUME] = {};
	float scale = 1.5f;
	int chunkSizeInt = static_cast<int>(CHUNK_SIZE);

	simplex->GenUniformGrid3D(gridArray, cx * chunkSizeInt * scale, cy * chunkSizeInt * scale, cz * chunkSizeInt * scale, chunkSizeInt, chunkSizeInt, chunkSizeInt, scale, scale, scale, 69);

	Chunk* chunk = new Chunk();
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {

			for (int y = 0; y < CHUNK_SIZE; y++) {
				float worldY = cy * static_cast<int>(CHUNK_SIZE) + y;

				float noiseValue = gridArray[(z * CHUNK_SIZE + y) * CHUNK_SIZE + x];
				float height = noiseValue * 100.0f;

				if (worldY < height - 5) {
					chunk->setBlockAt(getIndex(x, y, z), 3);
					continue;
				}
				else if (worldY < height) {
					chunk->setBlockAt(getIndex(x, y, z), 1);
				}
			}
		}
	}
	return chunk;
}