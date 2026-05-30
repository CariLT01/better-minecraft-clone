#pragma once

#include <array>
#include "config.h"

class Chunk {
public:
	Chunk();

	void setBlockAt(unsigned int index, uint8_t block);
	uint8_t getBlockAt(unsigned int index);

private:

	std::array<uint8_t, CHUNK_VOLUME> chunkData;
};