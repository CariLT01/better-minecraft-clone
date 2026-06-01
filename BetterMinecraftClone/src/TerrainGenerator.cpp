#include "TerrainGenerator.h"
#include "MathUtils.h"
#include <cmath>
#include <iostream>

TerrainGenerator::TerrainGenerator() {


	simplex2->SetSource(simplex);
	simplex2->SetWarpAmplitude(50.0f);
	caveFractal->SetSource(simplex2);
	caveFractal->SetOctaveCount(4);
}

TerrainGenerator::BorrowedBuffer TerrainGenerator::borrow() {
	std::unique_lock<std::mutex> lock(poolMu);
	std::unique_ptr<BufferType> rawBuffer;

	if (pool.empty()) {
		rawBuffer = std::make_unique<BufferType>();
		activeBuffersCount++;
		std::cout << "terrain gen buffer pool: created new instance. Buffers: "
			<< activeBuffersCount << std::endl;
	}
	else {
		rawBuffer = std::move(pool.front());
		pool.pop();
	}

	BufferType* rawPtr = rawBuffer.release();
	return BorrowedBuffer(rawPtr, [this](BufferType* ptr) {
		this->returnToPool(ptr);
		});

}

void TerrainGenerator::returnToPool(BufferType* rawPtr) {
	{
		std::unique_lock<std::mutex> lock(poolMu);
		pool.push(std::unique_ptr<BufferType>(rawPtr));
	}
}

std::shared_ptr<Chunk> TerrainGenerator::generateChunk(int cx, int cy, int cz) {

	float scale = 1.5f; // original: 1.5f
	int chunkSizeInt = static_cast<int>(CHUNK_SIZE);


	BorrowedBuffer gridArray1p = borrow();
	BorrowedBuffer gridArray2p = borrow();
	BorrowedBuffer gridArray3p = borrow();
	BorrowedBuffer gridArray4p = borrow();

	std::array<float, CHUNK_VOLUME>& gridArray1 = *gridArray1p;
	std::array<float, CHUNK_VOLUME>& gridArray2 = *gridArray2p;
	std::array<float, CHUNK_VOLUME>& gridArray3 = *gridArray3p;
	std::array<float, CHUNK_VOLUME>& gridArray4 = *gridArray4p;


	simplex->GenUniformGrid3D((gridArray1).data(), cx * chunkSizeInt * scale, cy * chunkSizeInt * scale, cz * chunkSizeInt * scale, chunkSizeInt, chunkSizeInt, chunkSizeInt, scale, scale, scale, 69);

	float caveScale = 1.5f; // original: 0.6f

	simplex->GenUniformGrid3D(gridArray2.data(), cx * chunkSizeInt * caveScale, cy * chunkSizeInt * caveScale, cz * chunkSizeInt * caveScale, chunkSizeInt, chunkSizeInt, chunkSizeInt, caveScale, caveScale, caveScale, 100);
	simplex->GenUniformGrid3D(gridArray3.data(), cx * chunkSizeInt * caveScale, cy * chunkSizeInt * caveScale, cz * chunkSizeInt * caveScale, chunkSizeInt, chunkSizeInt, chunkSizeInt, caveScale, caveScale, caveScale, 5);

	// HF noise
	float caveScaleHF = 10.0f;
	simplex->GenUniformGrid3D(gridArray4.data(), cx * chunkSizeInt * caveScaleHF, cy * chunkSizeInt * caveScaleHF, cz * chunkSizeInt * caveScaleHF, chunkSizeInt, chunkSizeInt, chunkSizeInt, caveScaleHF, caveScaleHF, caveScaleHF, 50);


	std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>();
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {

			for (int y = 0; y < CHUNK_SIZE; y++) {
				float worldY = cy * static_cast<int>(CHUNK_SIZE) + y;

				unsigned accessIndex = (z * CHUNK_SIZE + y) * CHUNK_SIZE + x;

				float noiseValue = gridArray1[accessIndex];
				float height = noiseValue * 100.0f;


				float hf = gridArray4[accessIndex];
				float tr = hf;
				float thr = std::max(0.02, hf * 0.06 + 0.04);

				bool cave1 = abs(gridArray2[accessIndex]) + tr * 0.02f < thr;
				bool cave2 = abs(gridArray3[accessIndex]) + tr * 0.02f < thr;

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