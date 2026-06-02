#pragma once

#include <functional>
#include <unordered_set>
#include "Chunk.h"
#include <glm/glm.hpp>
#include <optional>
#include "ChunkSectionView.h"
#include "ChunkMesh.h"
#include "ChunkBuilder.h"
#include "ShaderProgram.h"
#include "TerrainGenerator.h"
#include "TextureArray.h"
#include "ChunkBuilderWorkerScheduler.h"
#include "WorldGeneratorScheduler.h"
#include "Types.h"

constexpr unsigned int RENDER_DISTANCE_VOLUME = (RENDER_DISTANCE * 2 + 1) * (RENDER_DISTANCE * 2 + 1) * (RENDER_DISTANCE * 2 + 1);



class WorldChunks {
public:
	WorldChunks(std::shared_ptr<ShaderProgram> terrainShaderProgram, std::shared_ptr<TextureArray> textureAtlas);
	~WorldChunks();
    
    void update(glm::vec3 playerPosition);
    void render(std::shared_ptr<Camera> camera);

    BlockData getBlockAt(int x, int y, int z);
    void setBlockAt(int x, int y, int z, BlockData blockType);

private:
    std::unordered_map<ChunkPos, std::shared_ptr<Chunk>, ChunkPosHash> chunkMap;
    std::unordered_map<SectionPos, std::shared_ptr<ChunkSectionView>, SectionPosHash> chunkSections;
    std::unordered_map<SectionPos, std::shared_ptr<ChunkMesh>, SectionPosHash> chunkMeshesMap;

    std::unordered_set<SectionPos, SectionPosHash> loadedChunks;
    std::unordered_set<SectionPos, SectionPosHash> loadingChunks;
    std::unordered_set<SectionPos, SectionPosHash> pendingChunks;
    std::unordered_set<ChunkPos, ChunkPosHash> generatingChunks;
    

    std::array<ChunkPos, RENDER_DISTANCE_VOLUME> chunkOffsets;

    void precomputeChunkOffsets();

    bool isReadyToBuild(const SectionPos& pos);

	void buildChunkMesh(const SectionPos& pos);

    void recalculateLight(const ChunkPos& cpos, const std::shared_ptr<Chunk> chunk);

    std::optional<ChunkPos> getNextChunkToLoad(const ChunkPos& pos);

    std::shared_ptr<ShaderProgram> terrainShaderProgram;
    std::shared_ptr<TextureArray> textureAtlas;

    std::unique_ptr<WorldGeneratorScheduler> worldGenScheduler;

    unsigned int numTextures;

    std::unique_ptr<ChunkBuilderWorkerScheduler> scheduler;

    void remeshChunk(const SectionPos& pos);

    // Remesh with a block modification coordinate
    void remeshModified(const SectionPos& pos, int modificationX, int modificationY, int modificationZ);
  
    std::unordered_set<ChunkPos, ChunkPosHash> shouldBeLoadedLookup;


    std::unordered_map<ChunkPos, std::vector<LightStack>, ChunkPosHash> lightMapQueue;
};