#include <glm/glm.hpp>

inline int getChunkIndex(int x, int y, int z) {
    return x + (z * CHUNK_WIDTH) + (y * CHUNK_WIDTH * CHUNK_WIDTH);
}

inline int getSectionIndex(int x, int y, int z) {
    return x + (z * SECTION_SIZE) + (y * SECTION_SIZE * SECTION_SIZE);
}

inline glm::ivec3 getChunkXYZ(int index) {
    glm::ivec3 pos;
    pos.x = index % CHUNK_WIDTH;
    pos.z = (index / CHUNK_WIDTH) % CHUNK_WIDTH;
    pos.y = index / (CHUNK_WIDTH * CHUNK_WIDTH);

    return pos;
}

inline glm::ivec3 getSectionXYZ(int index) {
    glm::ivec3 pos;
    pos.x = index % SECTION_SIZE;
    pos.z = (index / SECTION_SIZE) % SECTION_SIZE;
    pos.y = index / (SECTION_SIZE * SECTION_SIZE);

    return pos;
}





inline glm::vec3 realPositionToBlockPosition(const glm::vec3& realPos) {
	return glm::floor(realPos);
}

inline glm::ivec2 blockPositionToChunkPosition(const glm::vec3& blockPos) {
	return glm::floor(glm::vec2(blockPos.x, blockPos.z) / (float)CHUNK_WIDTH);
}

inline glm::ivec2 realPositionToChunkPosition(const glm::vec3& realPos) {
	return blockPositionToChunkPosition(realPositionToBlockPosition(realPos));
}