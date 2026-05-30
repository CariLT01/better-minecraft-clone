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
	WorldChunks(ShaderProgram* terrainShaderProgram, TextureArray* textureAtlas);
	~WorldChunks();
    
    void update(glm::vec3 playerPosition);
    void render(Camera* camera);

private:
    std::unordered_map<ChunkPos, Chunk*, ChunkPosHash> chunkMap;
    std::unordered_map<ChunkPos, ChunkMesh*, ChunkPosHash> chunkMeshesMap;

    std::unordered_set<ChunkPos, ChunkPosHash> loadedChunks;
    std::unordered_set<ChunkPos, ChunkPosHash> pendingChunks;
    

    std::array<ChunkPos, RENDER_DISTANCE_VOLUME> chunkOffsets;

    void precomputeChunkOffsets();
    Chunk* generateChunk(const ChunkPos& pos);

    bool isReadyToBuild(const ChunkPos& pos);

	ChunkMesh* buildChunkMesh(const ChunkPos& pos);

    std::optional<ChunkPos> getNextChunkToLoad(const glm::vec3& playerChunkPos);

    ChunkBuilder* chunkBuilder;

    ShaderProgram* terrainShaderProgram;
    TextureArray* textureAtlas;

    TerrainGenerator* generator;

    unsigned int numTextures;
};