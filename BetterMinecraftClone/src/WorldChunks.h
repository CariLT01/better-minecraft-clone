#pragma once

#include <functional>
#include <unordered_set>
#include "Chunk.h"
#include <glm/glm.hpp>
#include <optional>
#include "ChunkMesh.h"
#include "ChunkBuilder.h"
#include "ShaderProgram.h"
#include "TerrainGenerator.h"
#include "TextureArray.h"
#include "ChunkBuilderWorkerScheduler.h"
#include "WorldGeneratorScheduler.h"

constexpr unsigned int RENDER_DISTANCE_VOLUME = (RENDER_DISTANCE * 2 + 1) * (RENDER_DISTANCE * 2 + 1) * (RENDER_DISTANCE * 2 + 1);

struct ChunkPos {
    int x, y, z;

    // Fast equality operator
    bool operator==(const ChunkPos& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

struct ChunkPosHash {
    size_t operator()(const ChunkPos& pos) const {
        size_t h1 = std::hash<int>{}(pos.x);
        size_t h2 = std::hash<int>{}(pos.y);
        size_t h3 = std::hash<int>{}(pos.z);
        // Standard bit-shifting to combine hashes
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

class WorldChunks {
public:
	WorldChunks(std::shared_ptr<ShaderProgram> terrainShaderProgram, std::shared_ptr<TextureArray> textureAtlas);
	~WorldChunks();
    
    void update(glm::vec3 playerPosition);
    void render(std::shared_ptr<Camera> camera);

    uint8_t getBlockAt(int x, int y, int z);
    void setBlockAt(int x, int y, int z, uint8_t blockType);

private:
    std::unordered_map<ChunkPos, std::shared_ptr<Chunk>, ChunkPosHash> chunkMap;
    std::unordered_map<ChunkPos, std::shared_ptr<ChunkMesh>, ChunkPosHash> chunkMeshesMap;

    std::unordered_set<ChunkPos, ChunkPosHash> loadedChunks;
    std::unordered_set<ChunkPos, ChunkPosHash> loadingChunks;
    std::unordered_set<ChunkPos, ChunkPosHash> pendingChunks;
    std::unordered_set<ChunkPos, ChunkPosHash> generatingChunks;
    

    std::array<ChunkPos, RENDER_DISTANCE_VOLUME> chunkOffsets;

    void precomputeChunkOffsets();
    std::shared_ptr<Chunk> generateChunk(const ChunkPos& pos);

    bool isReadyToBuild(const ChunkPos& pos);

	void buildChunkMesh(const ChunkPos& pos);

    std::optional<ChunkPos> getNextChunkToLoad(const glm::vec3& playerChunkPos);

    std::shared_ptr<ShaderProgram> terrainShaderProgram;
    std::shared_ptr<TextureArray> textureAtlas;

    std::unique_ptr<WorldGeneratorScheduler> worldGenScheduler;

    unsigned int numTextures;

    std::unique_ptr<ChunkBuilderWorkerScheduler> scheduler;

    void remeshChunk(const ChunkPos& pos);

    // Remesh with a block modification coordinate
    void remeshModified(const ChunkPos& pos, int modificationX, int modificationY, int modificationZ);
    
    std::unordered_set<ChunkPos, ChunkPosHash> shouldBeLoadedLookup;
};