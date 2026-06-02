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
    buildAoData();
}

ChunkBuilder::~ChunkBuilder() {

}

unsigned int ChunkBuilder::getAoIndex(int x, int y, int z, unsigned int face) {
    return (((x * 3 * 3) + y * 3) + z) * 6 + face;
}

void ChunkBuilder::buildAoData() {
    unsigned int index = 0;
    for (const AOPositionsData& aoPos : aoPositionData) {
        unsigned int faceIndex = index / 4;
        unsigned int i = getAoIndex(aoPos.vertexLocalPos.x, aoPos.vertexLocalPos.y, aoPos.vertexLocalPos.z, faceIndex);
        cachedAo[i] = aoPos;
        index++;
    }
}

std::vector<Vertex> ChunkBuilder::buildChunkMeshData(const ChunkDataInput& chunkDataIn) {

	// std::cout << "Rebuild" << std::endl;

	std::shared_ptr<ChunkSectionView> chunkData = chunkDataIn.chunkData;

	std::vector<Vertex> vertices;
    vertices.reserve(4096);

    for (unsigned int i = 0; i < SECTION_VOLUME; i++) {
        glm::ivec3 blockPos = getSectionXYZ(i);
        const int x = blockPos.x;
        const int y = blockPos.y;
        const int z = blockPos.z;
        BlockData blockType = chunkData->getBlockAt(x, y, z);
		if (blockType.blockId == 0) continue;
		for (int face = 0; face < 6; face++) {
            int ox = x + faceOffsetLookup[face * 3 + 0];
            int oy = y + faceOffsetLookup[face * 3 + 1];
            int oz = z + faceOffsetLookup[face * 3 + 2];
			if (!isVoid(chunkDataIn, ox, oy, oz)) continue;
			OffsetPositionLookup lk = getOffsetPositionLookup(chunkDataIn, ox, oy, oz);
			// std::cout << "Generate face at: " << x << ", " << y << ", " << z << " face: " << face << std::endl;
			generateFace(chunkDataIn, blockType, lk.chunkSection->getBlockAt(lk.lookupPos.x, lk.lookupPos.y, lk.lookupPos.z), face, x, y, z, vertices);
	    }
	}
	return vertices;
}

unsigned int packData(unsigned int textureType, unsigned int ao, unsigned int lightBlock, unsigned int lightSky) {

    // layout:
    // 8 bits texture type --- 8 bits light --- 2 bits ao

    uint8_t textureTypeMasked = textureType & 0b11111111;
    uint8_t lightMasked = ((lightBlock & 0b1111) << 4) | (lightSky & 0b1111);
    uint8_t aoMasked = ao & 0b11;

    unsigned int packed = (textureTypeMasked << 10) | (lightMasked << 2) | aoMasked;

    return packed;
}

unsigned int ChunkBuilder::getTextureTypeFromBlockTypeAndFace(uint8_t blockType, unsigned int faceIndex) {
    const auto& t = cachedBlockTypes.runtimeBlockTypes[static_cast<int>(blockType - 1)]; // TODO: stop repeatedly calling getRuntimeBlockTypes
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

unsigned int ChunkBuilder::getAoAtPosition(const ChunkDataInput& chunkData, int vx, int vy, int vz, unsigned int bx, unsigned int by, unsigned int bz, unsigned int face) {
    unsigned int i = getAoIndex(vx, vy, vz, face);

    AOPositionsData aoData = cachedAo[i];

    LocalPos side1Offset = aoData.blockAOpositions.side1;
    LocalPos side2Offset = aoData.blockAOpositions.side2;
    LocalPos side3Offset = aoData.blockAOpositions.side3;

    int ibx = static_cast<int>(bx);
    int iby = static_cast<int>(by);
    int ibz = static_cast<int>(bz);

    bool solidSide1 = !isVoid(chunkData, ibx + side1Offset.x, iby + side1Offset.y, ibz + side1Offset.z);
    bool solidSide2 = !isVoid(chunkData, ibx + side2Offset.x, iby + side2Offset.y, ibz + side2Offset.z);
    bool solidSide3 = !isVoid(chunkData, ibx + side3Offset.x, iby + side3Offset.y, ibz + side3Offset.z); // corner

    unsigned int ao = 0;
    if (solidSide1 && solidSide2) {
        ao = 0;
    }
    else {
        ao = 3 - (solidSide1 + solidSide2 + solidSide3);
    }

    return ao;
}

void ChunkBuilder::generateFace(const ChunkDataInput& chunkData, const BlockData& blockType, const BlockData& offsetBlockType, unsigned int faceIndex, unsigned int blockX, unsigned int blockY, unsigned int blockZ, std::vector<Vertex>& vertices) {
	// A face is made of 4 vertices (ordered as a quad)
		// We will convert them into 2 triangles (6 vertices total for OpenGL/DirectX)
    int uvLookup[4] = { 0, 1, 3, 2 };

	Vertex faceVertices[4];
    unsigned int aoValues[4];


    int ttype = getTextureTypeFromBlockTypeAndFace(blockType.blockId, faceIndex);;

	// 1. Gather the 4 corners for this face and add the block world position
	for (int i = 0; i < 4; i++) {
		int cornerIndex = faceTris[faceIndex][i];
		
        int localPosX = cubeVertices[cornerIndex][0];
        int localPosY = cubeVertices[cornerIndex][1];
        int localPosZ = cubeVertices[cornerIndex][2];

        aoValues[i] = getAoAtPosition(chunkData, localPosX, localPosY, localPosZ, blockX, blockY, blockZ, faceIndex);

        int vx = localPosX + blockX;
        int vy = localPosY + blockY;
        int vz = localPosZ + blockZ;

		 

		faceVertices[i].position = glm::vec3(vx, vy, vz);

		glm::vec3 normal = glm::vec3(normals[faceIndex][0], normals[faceIndex][1], normals[faceIndex][2]);

        faceVertices[i].normal = normal;
        //faceVertices[i].uv = glm::vec2(uvTemplate[uvLookup[i] * 2 + 0], uvTemplate[uvLookup[i] * 2 + 1]);
        faceVertices[i].uv = glm::vec2(uvTemplate[i * 2 + 0], uvTemplate[i * 2 + 1]);
        faceVertices[i].packedData = packData(ttype, aoValues[i], offsetBlockType.blockLight, offsetBlockType.skyLight);
	}

    int defaultIndices[6] = { 0, 1, 2, 2, 3, 0 };
    int flippedIndices[6] = { 1, 2, 3, 3, 0, 1 };

    int* chosenIndices = defaultIndices;
    if (aoValues[0] + aoValues[2] < aoValues[1] + aoValues[3]) {
        // std::cout << "Rotated" << std::endl;
        chosenIndices = flippedIndices;
    }

	// 2. Push the 6 vertices (2 triangles) into your mesh buffer
	for (int i = 0; i < 6; i++) {
		vertices.push_back(faceVertices[chosenIndices[i]]);
	}
}

OffsetPositionLookup ChunkBuilder::getOffsetPositionLookup(const ChunkDataInput& chunkData, int x, int y, int z) {
    std::shared_ptr<ChunkSectionView> chunkDataToCheck = chunkData.chunkData;


    if (x >= static_cast<int>(SECTION_SIZE)) {
        chunkDataToCheck = chunkData.right;
        x -= SECTION_SIZE;
    }
    else if (x < 0) {
        chunkDataToCheck = chunkData.left;
        x += SECTION_SIZE;
    }

    // 2. Check Y Boundaries independently 
    if (y >= static_cast<int>(SECTION_SIZE)) {
        chunkDataToCheck = chunkData.top;
        y -= SECTION_SIZE;
    }
    else if (y < 0) {
        chunkDataToCheck = chunkData.bottom;
        y += SECTION_SIZE;
    }

    // 3. Check Z Boundaries independently
    if (z >= static_cast<int>(SECTION_SIZE)) {
        chunkDataToCheck = chunkData.front;
        z -= SECTION_SIZE;
    }
    else if (z < 0) {
        chunkDataToCheck = chunkData.back;
        z += SECTION_SIZE;
    }

    // 4. Null safety guard (in case a neighboring chunk isn't loaded yet)
    if (!chunkDataToCheck) {
		// std::cout << "Warning: neighboring chunk section not loaded. Cannot perform block lookup for AO calculation. Returning default values." << std::endl;
        return {
			.lookupPos = {
				x, y, z
			},
            .chunkSection = chunkData.chunkData
        };
    }

    // 5. Final boundary confirmation & lookup
    if (x < 0 || x >= static_cast<int>(SECTION_SIZE) ||
        y < 0 || y >= static_cast<int>(SECTION_SIZE) ||
        z < 0 || z >= static_cast<int>(SECTION_SIZE)) {
		// std::cout << "Error: boundary check failed after adjusting for neighboring chunk sections. This should not happen. Check the logic in getOffsetPositionLookup." << std::endl;
        return {
            .lookupPos = {
                x, y, z
            },
            .chunkSection = chunkData.chunkData
        };
    }

    return {
        .lookupPos = {
            x, y, z
        },
        .chunkSection = chunkDataToCheck
    };

}

bool ChunkBuilder::isVoid(const ChunkDataInput& chunkData, int lx, int ly, int lz) {
	OffsetPositionLookup lookup = getOffsetPositionLookup(chunkData, lx, ly, lz);

	int x = lookup.lookupPos.x;
	int y = lookup.lookupPos.y;
	int z = lookup.lookupPos.z;

    // 1. Check X Boundaries independently



    return lookup.chunkSection->getBlockAt(x, y, z).blockId == 0;
}