#include "Chunk.h"

Chunk::Chunk() : chunkData() {

}

void Chunk::setBlockAt(unsigned int index, uint8_t block) {
	chunkData[index] = block;
}

uint8_t Chunk::getBlockAt(unsigned int index) {
	return chunkData[index];
}