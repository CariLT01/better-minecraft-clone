#pragma once

#include "Chunk.h"
#include "FastNoiseLite.h"
#include <FastNoise/FastNoise.h>
#include <memory>
#include <queue>
#include <mutex>

class TerrainGenerator {
public:
	TerrainGenerator();

	std::shared_ptr<Chunk> generateChunk(int x, int y, int z);

	using BufferType = std::array<float, CHUNK_VOLUME>;
	using BorrowedBuffer = std::unique_ptr<BufferType, std::function<void(BufferType*)>>;
private:

	FastNoise::SmartNode<FastNoise::Perlin> simplex = FastNoise::New<FastNoise::Perlin>();
	FastNoise::SmartNode<FastNoise::DomainWarpSimplex> simplex2 = FastNoise::New<FastNoise::DomainWarpSimplex>();
	FastNoise::SmartNode<FastNoise::Simplex> simplexbase = FastNoise::New<FastNoise::Simplex>();
	FastNoise::SmartNode<FastNoise::FractalFBm> caveFractal = FastNoise::New<FastNoise::FractalFBm>();

	std::queue<std::unique_ptr<BufferType>> pool;
	std::mutex poolMu;

	BorrowedBuffer borrow();
	void returnToPool(BufferType* rawPtr);
	unsigned int activeBuffersCount = 0;
};