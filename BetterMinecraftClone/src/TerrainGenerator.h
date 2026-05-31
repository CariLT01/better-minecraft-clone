#pragma once

#include "Chunk.h"
#include "FastNoiseLite.h"
#include <FastNoise/FastNoise.h>

class TerrainGenerator {
public:
	TerrainGenerator();

	Chunk* generateChunk(int x, int y, int z);
private:

	FastNoise::SmartNode<FastNoise::Perlin> simplex = FastNoise::New<FastNoise::Perlin>();
};