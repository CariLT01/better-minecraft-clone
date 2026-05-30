#include "TerrainGenerator.h"
#include "MathUtils.h"

TerrainGenerator::TerrainGenerator() : noise(new FastNoiseLite()) {
	noise->SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
	noise->SetFractalType(FastNoiseLite::FractalType_FBm);
	noise->SetFractalOctaves(4);
}

Chunk* TerrainGenerator::generateChunk(int cx, int cy, int cz) {

	Chunk* chunk = new Chunk();
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {

			float worldX = cx * static_cast<int>(CHUNK_SIZE) + x;
			float worldZ = cz * static_cast<int>(CHUNK_SIZE) + z;



			for (int y = 0; y < CHUNK_SIZE; y++) {
				
				
				float worldY = cy * static_cast<int>(CHUNK_SIZE) + y;

				float noiseValue = noise->GetNoise(worldX * 0.5f, worldY * 0.5f, worldZ * 0.5f);
				float height = noiseValue * 100.0f;

				if (worldY < height) {
					chunk->setBlockAt(getIndex(x, y, z), 1);
				}
			}
		}
	}
	return chunk;
}