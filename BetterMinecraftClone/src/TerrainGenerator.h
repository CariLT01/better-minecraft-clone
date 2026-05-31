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
	FastNoise::SmartNode<FastNoise::DomainWarpSimplex> simplex2 = FastNoise::New<FastNoise::DomainWarpSimplex>();
	FastNoise::SmartNode<FastNoise::Simplex> simplexbase = FastNoise::New<FastNoise::Simplex>();
	FastNoise::SmartNode<FastNoise::FractalFBm> caveFractal = FastNoise::New<FastNoise::FractalFBm>();

	std::array<float, CHUNK_VOLUME> gridArray1;
	std::array<float, CHUNK_VOLUME> gridArray2;
	std::array<float, CHUNK_VOLUME> gridArray3;
};