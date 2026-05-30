#pragma once

#include "ChunkMesh.h"
#include "config.h"
#include "Chunk.h"

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
    void generateFace(unsigned int faceIndex, unsigned int blockX, unsigned int blockY, unsigned int blockZ, std::vector<Vertex>& vertices);

    bool isVoid(const ChunkDataInput& chunkData, int blockX, int blockY, int blockZ);
};