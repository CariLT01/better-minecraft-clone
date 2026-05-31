#pragma once

#include "ChunkMesh.h"
#include "config.h"
#include "Chunk.h"
#include "BlockTypes.h"

struct LocalPos {
    int x;
    int y;
    int z;
};

struct BlockAOPositions {
    LocalPos side1; // block position (offset)
    LocalPos side2; // block position (offset)
    LocalPos side3; // block position (offset)
};

struct AOPositionsData {
    LocalPos vertexLocalPos; // local position of the current vertex (see below)
    BlockAOPositions blockAOpositions; // positions offset of the current block to check
};

constexpr float cubeVertices[8][3] = {
    {0.0f, 0.0f, 0.0f}, // 0
    {1.0f, 0.0f, 0.0f}, // 1
    {1.0f, 0.0f, 1.0f}, // 2
    {0.0f, 0.0f, 1.0f}, // 3
    {0.0f, 1.0f, 0.0f}, // 4
    {1.0f, 1.0f, 0.0f}, // 5
    {1.0f, 1.0f, 1.0f}, // 6
    {0.0f, 1.0f, 1.0f}  // 7
};

constexpr unsigned int aoPossibilities = 3 * 3 * 3 * 6;

/*
0	Front	[ 0.0, 0.0, 1.0] (Towards you)	faceTris[0] -> {3, 2, 6, 7}
1	Back	[ 0.0, 0.0, -1.0] (Away from you)	faceTris[1] -> {1, 0, 4, 5}
2	Left	[-1.0, 0.0, 0.0] (To the left)	faceTris[2] -> {0, 3, 7, 4}
3	Right	[ 1.0, 0.0, 0.0] (To the right)	faceTris[3] -> {2, 1, 5, 6}
4	Top	[ 0.0, 1.0, 0.0] (Up)	faceTris[4] -> {4, 5, 6, 7}
5	Bottom	[ 0.0, -1.0, 0.0] (Down)	faceTris[5] -> {1, 2, 3, 0}
*/

constexpr AOPositionsData aoPositionData[6 * 4] = {
    // ==========================================
    // 0: FRONT FACE [0, 0, 1] (Vertices: 3, 2, 6, 7)
    // ==========================================
    {
        {0, 0, 1}, // Vertex 3
        { {-1, 0, 1}, {0, -1, 1}, {-1, -1, 1} }
    },
    {
        {1, 0, 1}, // Vertex 2
        { {1, 0, 1}, {0, -1, 1}, {1, -1, 1} }
    },
    {
        {1, 1, 1}, // Vertex 6
        { {1, 0, 1}, {0, 1, 1}, {1, 1, 1} }
    },
    {
        {0, 1, 1}, // Vertex 7
        { {-1, 0, 1}, {0, 1, 1}, {-1, 1, 1} }
    },

    // ==========================================
    // 1: BACK FACE [0, 0, -1] (Vertices: 1, 0, 4, 5)
    // ==========================================
    {
        {1, 0, 0}, // Vertex 1
        { {1, 0, -1}, {0, -1, -1}, {1, -1, -1} }
    },
    {
        {0, 0, 0}, // Vertex 0
        { {-1, 0, -1}, {0, -1, -1}, {-1, -1, -1} }
    },
    {
        {0, 1, 0}, // Vertex 4
        { {-1, 0, -1}, {0, 1, -1}, {-1, 1, -1} }
    },
    {
        {1, 1, 0}, // Vertex 5
        { {1, 0, -1}, {0, 1, -1}, {1, 1, -1} }
    },

    // ==========================================
    // 2: LEFT FACE [-1, 0, 0] (Vertices: 0, 3, 7, 4)
    // ==========================================
    {
        {0, 0, 0}, // Vertex 0
        { {-1, 0, -1}, {-1, -1, 0}, {-1, -1, -1} }
    },
    {
        {0, 0, 1}, // Vertex 3
        { {-1, 0, 1}, {-1, -1, 0}, {-1, -1, 1} }
    },
    {
        {0, 1, 1}, // Vertex 7
        { {-1, 0, 1}, {-1, 1, 0}, {-1, 1, 1} }
    },
    {
        {0, 1, 0}, // Vertex 4
        { {-1, 0, -1}, {-1, 1, 0}, {-1, 1, -1} }
    },

    // ==========================================
    // 3: RIGHT FACE [1, 0, 0] (Vertices: 2, 1, 5, 6)
    // ==========================================
    {
        {1, 0, 1}, // Vertex 2
        { {1, 0, 1}, {1, -1, 0}, {1, -1, 1} }
    },
    {
        {1, 0, 0}, // Vertex 1
        { {1, 0, -1}, {1, -1, 0}, {1, -1, -1} }
    },
    {
        {1, 1, 0}, // Vertex 5
        { {1, 0, -1}, {1, 1, 0}, {1, 1, -1} }
    },
    {
        {1, 1, 1}, // Vertex 6
        { {1, 0, 1}, {1, 1, 0}, {1, 1, 1} }
    },

    // ==========================================
    // 4: TOP FACE [0, 1, 0] (Vertices: 4, 5, 6, 7)
    // ==========================================
    {
        {0, 1, 0}, // Vertex 4
        { {-1, 1, 0}, {0, 1, -1}, {-1, 1, -1} }
    },
    {
        {1, 1, 0}, // Vertex 5
        { {1, 1, 0}, {0, 1, -1}, {1, 1, -1} }
    },
    {
        {1, 1, 1}, // Vertex 6
        { {1, 1, 0}, {0, 1, 1}, {1, 1, 1} }
    },
    {
        {0, 1, 1}, // Vertex 7
        { {-1, 1, 0}, {0, 1, 1}, {-1, 1, 1} }
    },

    // ==========================================
    // 5: BOTTOM FACE [0, -1, 0] (Vertices: 1, 2, 3, 0)
    // ==========================================
    {
        {1, 0, 0}, // Vertex 1
        { {1, -1, 0}, {0, -1, -1}, {1, -1, -1} }
    },
    {
        {1, 0, 1}, // Vertex 2
        { {1, -1, 0}, {0, -1, 1}, {1, -1, 1} }
    },
    {
        {0, 0, 1}, // Vertex 3
        { {-1, -1, 0}, {0, -1, 1}, {-1, -1, 1} }
    },
    {
        {0, 0, 0}, // Vertex 0
        { {-1, -1, 0}, {0, -1, -1}, {-1, -1, -1} }
    }
};

const int faceTris[6][4] = {
    {3, 2, 6, 7}, // Front (+Z)
    {1, 0, 4, 5}, // Back (-Z)
    {0, 3, 7, 4}, // Left (-X)
    {2, 1, 5, 6}, // Right (+X)
    {7, 6, 5, 4}, // Top (+Y)
    {1, 2, 3, 0}  // Bottom (-Y)
};

const float normals[6][3] = {
    { 0.0f,  0.0f,  1.0f}, // 0: Front
    { 0.0f,  0.0f, -1.0f}, // 1: Back
    {-1.0f,  0.0f,  0.0f}, // 2: Left
    { 1.0f,  0.0f,  0.0f}, // 3: Right
    { 0.0f,  1.0f,  0.0f}, // 4: Top
    { 0.0f, -1.0f,  0.0f}  // 5: Bottom
};

constexpr float uvTemplate[4 * 2] = {
    0.0f, 1.0f, // Was Bottom-Left, now Top-Left
    1.0f, 1.0f, // Was Bottom-Right, now Top-Right
    1.0f, 0.0f, // Was Top-Right, now Bottom-Right
    0.0f, 0.0f  // Was Top-Left, now Bottom-Left
};



struct ChunkDataInput {
	Chunk* chunkData;
    // surrounding sides
    Chunk* front;
    Chunk* back;
    Chunk* left;
    Chunk* right;
    Chunk* top;
    Chunk* bottom;
};

class ChunkBuilder {
public:
	ChunkBuilder();
	~ChunkBuilder();

	std::vector<Vertex> buildChunkMeshData(const ChunkDataInput& chunkData);
private:
    void generateFace(const ChunkDataInput& chunkData, uint8_t blockType, unsigned int faceIndex, unsigned int blockX, unsigned int blockY, unsigned int blockZ, std::vector<Vertex>& vertices);

    bool isVoid(const ChunkDataInput& chunkData, int blockX, int blockY, int blockZ);

    unsigned int getTextureTypeFromBlockTypeAndFace(uint8_t blockType, unsigned int faceIndex);

    std::array<AOPositionsData, aoPossibilities> cachedAo;
    unsigned int getAoIndex(int x, int y, int z, unsigned int face);

    unsigned int getAoAtPosition(const ChunkDataInput& chunkData, int vx, int vy, int vz, unsigned int bx, unsigned int by, unsigned int bz, unsigned int face);

    void buildAoData();

    CompileTimeResult cachedBlockTypes;
};