#include "Chunk.h"
#include "Types.h"
#include "MathUtils.h"
#include <cassert>
#include <queue>
#include <iostream>
#include <unordered_set>

Chunk::Chunk() : chunkData() {

}

void Chunk::setBlockAt(unsigned int index, BlockData blockData) {
	chunkData[index] = pack(blockData.blockId, blockData.blockLight, blockData.skyLight);
}

void Chunk::setBlockDataAt(unsigned int index, uint16_t data) {
	chunkData[index] = data;
}

uint16_t Chunk::getBlockDataAt(unsigned int index) {
#if NDEBUG
#else
	if (index >= chunkData.size()) {
		assert("index out of bounds");
	}
#endif
	return chunkData[index];
}

BlockData Chunk::getBlockAt(unsigned int index) {
	uint16_t data = getBlockDataAt(index);
	return unpack(data);
}

std::unordered_map<ChunkPos, std::vector<LightStack>, ChunkPosHash> Chunk::calculateLight(const std::vector<LightStack>& startLightStack, int cx, int cz) {

	std::cout << "Began building light" << std::endl;

	std::unordered_map<ChunkPos, std::vector<LightStack>, ChunkPosHash> lightMapBorderQueue;
	std::queue<LightStack> lightQueue;

	for (const LightStack& stack : startLightStack) {
		lightQueue.push(stack);
	}

	// from the top of the world, scan

	for (int x = 0; x < CHUNK_WIDTH; x++) {
		for (int z = 0; z < CHUNK_WIDTH; z++) {
			for (int y = CHUNK_HEIGHT - 1; y >= 0; y--) {
				BlockData block = getBlockAt(getChunkIndex(x, y, z));

				if (block.blockId != 0) {
					if (y == CHUNK_HEIGHT - 1) break;
					lightQueue.push({ { x, y + 1, z } , 15 });
					break;
				}

				
			}
		}
	}

	// propagate

	while (!lightQueue.empty()) {
		LightStack blockPos = lightQueue.front();
		lightQueue.pop();
		unsigned int i = getChunkIndex(blockPos.pos.x, blockPos.pos.y, blockPos.pos.z);

		for (int i = 0; i < 6; i++) {
			int offX = offsets[i * 3 + 0];
			int offY = offsets[i * 3 + 1];
			int offZ = offsets[i * 3 + 2];

			int bx = blockPos.pos.x + offX;
			int by = blockPos.pos.y + offY;
			int bz = blockPos.pos.z + offZ;

			unsigned int idx = getChunkIndex(bx, by, bz);

			

			if (by < 0 || by >= CHUNK_HEIGHT) continue;

			// check out of bounds
			if (bx >= CHUNK_WIDTH || bz >= CHUNK_WIDTH) {
				// handle out of bounds

				ChunkPos borderChunk = { cx + offX, cz + offZ };

				if (lightMapBorderQueue.find(borderChunk) == lightMapBorderQueue.end()) {
					lightMapBorderQueue[borderChunk] = {};
				}
				int lx = (bx % CHUNK_WIDTH + CHUNK_WIDTH) % CHUNK_WIDTH;
				int ly = by;
				int lz = (bz % CHUNK_WIDTH + CHUNK_WIDTH) % CHUNK_WIDTH;

				lightMapBorderQueue[borderChunk].push_back({
					.pos = { lx, ly, lz },
					.lightLevel = blockPos.lightLevel
					});
				continue;
			}

			if (bx < 0 || bz < 0) {
				ChunkPos borderChunk = { cx + offX, cz + offZ };

				if (lightMapBorderQueue.find(borderChunk) == lightMapBorderQueue.end()) {
					lightMapBorderQueue[borderChunk] = {};
				}
				int lx = (bx % CHUNK_WIDTH + CHUNK_WIDTH) % CHUNK_WIDTH;
				int ly = by;
				int lz = (bz % CHUNK_WIDTH + CHUNK_WIDTH) % CHUNK_WIDTH;
				lightMapBorderQueue[borderChunk].push_back({
					.pos = { lx, ly, lz },
					.lightLevel = blockPos.lightLevel
					});
				continue;
			}


			BlockData bdata = getBlockAt(getChunkIndex(bx, by, bz));

			uint8_t targetLight = blockPos.lightLevel - 1;
			if (targetLight <= bdata.skyLight) continue;


			setBlockAt(getChunkIndex(bx, by, bz), {
				.blockId = bdata.blockId,
				.skyLight = targetLight,
				.blockLight = bdata.blockLight
			});

			lightQueue.push({ { bx, by, bz }, targetLight });
		}
	}

	std::cout << "Finished building light" << std::endl;

	return lightMapBorderQueue;
}