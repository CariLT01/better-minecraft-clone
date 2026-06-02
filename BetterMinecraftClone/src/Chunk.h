#pragma once

#include <array>
#include <unordered_map>
#include "config.h"
#include "Types.h"

struct BlockData {
	uint8_t blockId;
	uint8_t skyLight;
	uint8_t blockLight;
};

constexpr int offsets[] = {
	0, 0, 1, // front
	0, 0, -1, // back
	-1, 0, 0, // left
	1, 0, 0, // right
	0, 1, 0, // top
	0, -1, 0 // bottom
};

struct LightStack {
	SectionPos pos;
	uint8_t lightLevel;
};

class Chunk {
public:
	Chunk();

	void setBlockAt(unsigned int index, BlockData data);
	void setBlockDataAt(unsigned int index, uint16_t data);
	BlockData getBlockAt(unsigned int index);
	uint16_t getBlockDataAt(unsigned int index);

	static uint16_t pack(uint8_t blockId, uint8_t blockLight, uint8_t skyLight) {
		// BBBBBBBBLLLLSSSS
		return (blockId << 8) | ((blockLight & 0b1111) << 4) | (skyLight & 0b1111);
	}

	static BlockData unpack(uint16_t blockData) {
		BlockData data;
		data.blockId = (blockData >> 8) & 0xFF;
		data.blockLight = (blockData >> 4) & 0b1111;
		data.skyLight = blockData & 0b1111;
		return data;
	}

	std::unordered_map<ChunkPos, std::vector<LightStack>, ChunkPosHash> calculateLight(const std::vector<LightStack>& initialLightStack, int cx, int cz);

private:

	std::array<uint16_t, CHUNK_VOLUME> chunkData;
};