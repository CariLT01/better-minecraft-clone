#include <glm/glm.hpp>

inline int getIndex(int x, int y, int z) {
    return x + (z * CHUNK_SIZE) + (y * CHUNK_SIZE * CHUNK_SIZE);
}

inline glm::vec3 getXYZ(int index) {
    glm::vec3 pos;
    pos.x = index % CHUNK_SIZE;
    pos.z = (index / CHUNK_SIZE) % CHUNK_SIZE;
    pos.y = index / (CHUNK_SIZE * CHUNK_SIZE);

    return pos;
}

inline glm::vec3 realPositionToBlockPosition(const glm::vec3& realPos) {
	return glm::floor(realPos);
}

inline glm::vec3 blockPositionToChunkPosition(const glm::vec3& blockPos) {
	return glm::floor(blockPos / (float)CHUNK_SIZE);
}

inline glm::vec3 realPositionToChunkPosition(const glm::vec3& realPos) {
	return blockPositionToChunkPosition(realPositionToBlockPosition(realPos));
}