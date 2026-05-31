#include "Chunk.h"
#include <cassert>

Chunk::Chunk() : chunkData() {

}

void Chunk::setBlockAt(unsigned int index, uint8_t block) {
	chunkData[index] = block;
}

uint8_t Chunk::getBlockAt(unsigned int index) {
#if NDEBUG
#else
	if (index >= chunkData.size()) {
		assert("index out of bounds");
	}
#endif
	return chunkData[index];
}