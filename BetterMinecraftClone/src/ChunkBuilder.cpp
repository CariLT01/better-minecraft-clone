#include "ChunkBuilder.h"
#include "MathUtils.h"
#include <array>
#include <iostream>

/* 
0	Front	[ 0.0, 0.0, 1.0] (Towards you)	faceTris[0] -> {3, 2, 6, 7}
1	Back	[ 0.0, 0.0, -1.0] (Away from you)	faceTris[1] -> {1, 0, 4, 5}
2	Left	[-1.0, 0.0, 0.0] (To the left)	faceTris[2] -> {0, 3, 7, 4}
3	Right	[ 1.0, 0.0, 0.0] (To the right)	faceTris[3] -> {2, 1, 5, 6}
4	Top	[ 0.0, 1.0, 0.0] (Up)	faceTris[4] -> {4, 5, 6, 7}
5	Bottom	[ 0.0, -1.0, 0.0] (Down)	faceTris[5] -> {1, 2, 3, 0}
*/

const int faceOffsetLookup[6 * 3] = {
	0, 0, 1, // front
	0, 0, -1, // back
	-1, 0, 0, // left
	1, 0, 0, // right
	0, 1, 0, // top
	0, -1, 0 // bottom
};




ChunkBuilder::ChunkBuilder() {
    cachedBlockTypes = getRuntimeBlockTypes();
}

std::vector<Vertex> ChunkBuilder::buildChunkMeshData(const ChunkDataInput& chunkDataIn) {

	std::cout << "Rebuild" << std::endl;

	Chunk* chunkData = chunkDataIn.chunkData;

	std::vector<Vertex> vertices;
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int z = 0; z < CHUNK_SIZE; z++) {
				int blockIndex = getIndex(x, y, z);
                uint8_t blockType = chunkData->getBlockAt(blockIndex);
				if (blockType == 0) continue;

				for (int face = 0; face < 6; face++) {
					if (!isVoid(chunkDataIn, x + faceOffsetLookup[face * 3 + 0], y + faceOffsetLookup[face * 3 + 1], z + faceOffsetLookup[face * 3 + 2])) continue;
					// std::cout << "Generate face at: " << x << ", " << y << ", " << z << " face: " << face << std::endl;
					generateFace(blockType, face, x, y, z, vertices);
				}
			}
		}
	}
	return vertices;
}

unsigned int packData(unsigned int blockType, unsigned int faceIndex, unsigned int vertexIndex) {

    unsigned int maskedIndex = vertexIndex & 0x03;   // 2 bits max (0-3)
    unsigned int maskedFace = faceIndex & 0x07;     // 3 bits max (0-7)
    unsigned int maskedBlock = blockType & 0xFF;     // 8 bits max (0-255)

	return (maskedBlock << 5) | (maskedFace << 2) | maskedIndex;
}

unsigned int getTextureTypeFromBlockTypeAndFace(uint8_t blockType, unsigned int faceIndex) {
    RuntimeBlockType t = getRuntimeBlockTypes().runtimeBlockTypes[static_cast<int>(blockType - 1)]; // TODO: stop repeatedly calling getRuntimeBlockTypes
    if (faceIndex == 4) {
		// top face
		return t.topTextureIndex;
	}
	else if (faceIndex == 5) {
		// bottom face
		return t.bottomTextureIndex;
	}
    else {
        // side faces
        return t.sideTextureIndex;
    }
}

void ChunkBuilder::generateFace(uint8_t blockType, unsigned int faceIndex, unsigned int blockX, unsigned int blockY, unsigned int blockZ, std::vector<Vertex>& vertices) {
	// A face is made of 4 vertices (ordered as a quad)
		// We will convert them into 2 triangles (6 vertices total for OpenGL/DirectX)
	int indices[6] = { 0, 1, 2, 2, 3, 0 };
    int uvLookup[4] = { 0, 1, 3, 2 };

	Vertex faceVertices[4];

    int ttype = getTextureTypeFromBlockTypeAndFace(blockType, faceIndex);;

	// 1. Gather the 4 corners for this face and add the block world position
	for (int i = 0; i < 4; i++) {
		int cornerIndex = faceTris[faceIndex][i];
		
		glm::vec3 vertexPos = glm::vec3(cubeVertices[cornerIndex][0] + blockX, cubeVertices[cornerIndex][1] + blockY, cubeVertices[cornerIndex][2] + blockZ);

		faceVertices[i].position = vertexPos;

		glm::vec3 normal = glm::vec3(normals[faceIndex][0], normals[faceIndex][1], normals[faceIndex][2]);

        faceVertices[i].normal = normal;
        //faceVertices[i].uv = glm::vec2(uvTemplate[uvLookup[i] * 2 + 0], uvTemplate[uvLookup[i] * 2 + 1]);
        faceVertices[i].uv = glm::vec2(uvTemplate[i * 2 + 0], uvTemplate[i * 2 + 1]);
        faceVertices[i].textureType = ttype;
	}

	// 2. Push the 6 vertices (2 triangles) into your mesh buffer
	for (int i = 0; i < 6; i++) {
		vertices.push_back(faceVertices[indices[i]]);
	}
}

bool ChunkBuilder::isVoid(const ChunkDataInput& chunkData, int x, int y, int z) {
    Chunk* chunkDataToCheck = chunkData.chunkData;

    // 1. Check X Boundaries independently
    if (x >= static_cast<int>(CHUNK_SIZE)) {
        chunkDataToCheck = chunkData.right;
        x -= CHUNK_SIZE;
    }
    else if (x < 0) {
        chunkDataToCheck = chunkData.left;
        x += CHUNK_SIZE;
    }

    // 2. Check Y Boundaries independently 
    if (y >= static_cast<int>(CHUNK_SIZE)) {
        chunkDataToCheck = chunkData.top;
        y -= CHUNK_SIZE;
    }
    else if (y < 0) {
        chunkDataToCheck = chunkData.bottom;
        y += CHUNK_SIZE;
    }

    // 3. Check Z Boundaries independently
    if (z >= static_cast<int>(CHUNK_SIZE)) {
        chunkDataToCheck = chunkData.front;
        z -= CHUNK_SIZE;
    }
    else if (z < 0) {
        chunkDataToCheck = chunkData.back;
        z += CHUNK_SIZE;
    }

    // 4. Null safety guard (in case a neighboring chunk isn't loaded yet)
    if (!chunkDataToCheck) {
        return true; // Treat unloaded chunks as empty space/void
    }

    // 5. Final boundary confirmation & lookup
    if (x < 0 || x >= static_cast<int>(CHUNK_SIZE) ||
        y < 0 || y >= static_cast<int>(CHUNK_SIZE) ||
        z < 0 || z >= static_cast<int>(CHUNK_SIZE)) {
        return true;
    }

    unsigned int index = getIndex(static_cast<unsigned int>(x),
        static_cast<unsigned int>(y),
        static_cast<unsigned int>(z));

    return chunkDataToCheck->getBlockAt(index) == 0;
}