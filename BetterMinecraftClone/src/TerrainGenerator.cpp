#include "TerrainGenerator.h"
#include "MathUtils.h"
#include <cmath>

TerrainGenerator::TerrainGenerator() {


	simplex2->SetSource(simplex);
	simplex2->SetWarpAmplitude(50.0f);
	caveFractal->SetSource(simplex2);
	caveFractal->SetOctaveCount(4);
}

Chunk* TerrainGenerator::generateChunk(int cx, int cy, int cz) {

	float gridArray[CHUNK_VOLUME] = {};
	float scale = 1.5f;
	int chunkSizeInt = static_cast<int>(CHUNK_SIZE);

	simplex->GenUniformGrid3D(gridArray, cx * chunkSizeInt * scale, cy * chunkSizeInt * scale, cz * chunkSizeInt * scale, chunkSizeInt, chunkSizeInt, chunkSizeInt, scale, scale, scale, 69);

	float caveGridArray[CHUNK_VOLUME] = {};
	float caveGridArray2[CHUNK_VOLUME] = {};
	float caveScale = 0.6f;

	simplex2->GenUniformGrid3D(caveGridArray, cx * chunkSizeInt * caveScale, cy * chunkSizeInt * caveScale, cz * chunkSizeInt * caveScale, chunkSizeInt, chunkSizeInt, chunkSizeInt, caveScale, caveScale, caveScale, 100);
	simplex2->GenUniformGrid3D(caveGridArray2, cx * chunkSizeInt * caveScale, cy * chunkSizeInt * caveScale, cz * chunkSizeInt * caveScale, chunkSizeInt, chunkSizeInt, chunkSizeInt, caveScale, caveScale, caveScale, 5);



	Chunk* chunk = new Chunk();
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {

			for (int y = 0; y < CHUNK_SIZE; y++) {
				float worldY = cy * static_cast<int>(CHUNK_SIZE) + y;

				unsigned accessIndex = (z * CHUNK_SIZE + y) * CHUNK_SIZE + x;

				float noiseValue = gridArray[accessIndex];
				float height = noiseValue * 100.0f;

				bool cave1 = abs(caveGridArray[accessIndex]) < 0.04f;
				bool cave2 = abs(caveGridArray2[accessIndex]) < 0.04f;

				if (cave1 && cave2) {
					// leave air
					continue;
				}



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