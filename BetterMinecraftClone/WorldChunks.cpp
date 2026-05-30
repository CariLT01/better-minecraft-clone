#include "WorldChunks.h"
#include "MathUtils.h"
#include <iostream>
#include <algorithm>

WorldChunks::WorldChunks(ShaderProgram* terrainShaderProgram) : chunkMap(), loadedChunks(), pendingChunks(), chunkOffsets(), chunkMeshesMap(), chunkBuilder(new ChunkBuilder()),
terrainShaderProgram(terrainShaderProgram), generator(new TerrainGenerator()) {
	precomputeChunkOffsets();
}


void WorldChunks::update(glm::vec3 playerPosition) {
	glm::vec3 chunkPosition = realPositionToChunkPosition(playerPosition);

	std::optional<ChunkPos> nextChunkToLoad = getNextChunkToLoad(chunkPosition);
	if (!nextChunkToLoad.has_value()) {
		std::cout << "No more chunks to load" << std::endl;
		return;
	}

	// Generate chunk if it doesn't exist
	if (chunkMap.find(nextChunkToLoad.value()) == chunkMap.end()) {
		Chunk* newChunk = generateChunk(nextChunkToLoad.value());

		chunkMap[nextChunkToLoad.value()] = newChunk;
	}

	// Add it to pending if not already in it
	if (pendingChunks.find(nextChunkToLoad.value()) == pendingChunks.end()) {
		pendingChunks.insert(nextChunkToLoad.value());
	}

	std::unordered_set<ChunkPos, ChunkPosHash> chunksToRemove;

	// Process already pending chunks
	for (const ChunkPos& pendingPos : pendingChunks) {
		if (isReadyToBuild(pendingPos)) {
			ChunkMesh* chunkMesh = buildChunkMesh(pendingPos);
			chunkMeshesMap[pendingPos] = chunkMesh;
			loadedChunks.insert(pendingPos);
			chunksToRemove.insert(pendingPos);
		}
	}

	for (const ChunkPos& pos : chunksToRemove) {
		pendingChunks.erase(pos);
	}

	chunksToRemove.clear();

	// Handle unloading

	// Build a fast lookup of should be currently loaded chunks
	std::unordered_set<ChunkPos, ChunkPosHash> shouldBeLoadedLookup;

	for (unsigned int i = 0; i < RENDER_DISTANCE_VOLUME; i++) {
		ChunkPos candidatePos = {
			chunkPosition.x + chunkOffsets[i].x,
			chunkPosition.y + chunkOffsets[i].y,
			chunkPosition.z + chunkOffsets[i].z
		};
		shouldBeLoadedLookup.insert(candidatePos);
	}

	for (const ChunkPos& pos : loadedChunks) {
		// check if it's in chunk offsets
		if (shouldBeLoadedLookup.find(pos) == shouldBeLoadedLookup.end()) {
			// Unload it
			delete chunkMeshesMap[pos];
			chunkMeshesMap.erase(pos);
			// chunkMap.erase(pos);
			chunksToRemove.insert(pos);
		}
	}
	for (const ChunkPos& pos : chunksToRemove) {
		loadedChunks.erase(pos);
	}

	
}

ChunkMesh* WorldChunks::buildChunkMesh(const ChunkPos& pos) {
	Chunk* chunkData = chunkMap[pos];

	ChunkDataInput chunkDataIn = {
		.chunkData = chunkData,
		.front = chunkMap[{pos.x, pos.y, pos.z + 1}],
		.back = chunkMap[{pos.x, pos.y, pos.z - 1}],
		.left = chunkMap[{pos.x - 1, pos.y, pos.z}],
		.right = chunkMap[{pos.x + 1, pos.y, pos.z}],
		.top = chunkMap[{pos.x, pos.y + 1, pos.z}],
		.bottom = chunkMap[{pos.x, pos.y - 1, pos.z}]
	};

	std::vector<Vertex> chunkVertices = chunkBuilder->buildChunkMeshData(chunkDataIn);
	glm::vec3 cposGlm = glm::vec3(pos.x, pos.y, pos.z);
	ChunkMesh* newChunkMesh = new ChunkMesh(chunkVertices, {}, terrainShaderProgram, cposGlm * glm::vec3(CHUNK_SIZE));
	return newChunkMesh;
}

constexpr int neighbors[6 * 3] = {
	0, 0, 1, // front
	0, 0, -1, // back
	-1, 0, 0, // left
	1, 0, 0, // right
	0, 1, 0, // top
	0, -1, 0 // bottom
};

bool WorldChunks::isReadyToBuild(const ChunkPos& pos) {
	for (unsigned int i = 0; i < 6; i++) {
		ChunkPos neighborPos = {
			pos.x + neighbors[i * 3 + 0],
			pos.y + neighbors[i * 3 + 1],
			pos.z + neighbors[i * 3 + 2]
		};
		if (chunkMap.find(neighborPos) == chunkMap.end()) return false;
	}
	return true;
}

void WorldChunks::precomputeChunkOffsets() {
	unsigned int i = 0;
	int s_render_distance = static_cast<int>(RENDER_DISTANCE);

	// 1. Populate the offsets array as normal
	for (int x = -s_render_distance; x < s_render_distance; x++) {
		for (int y = -s_render_distance; y < s_render_distance; y++) {
			for (int z = -s_render_distance; z < s_render_distance; z++) {
				chunkOffsets[i] = { x, y, z };
				i++;
			}
		}
	}

	// 2. Sort the array so the closest chunks to (0,0,0) come first
	// This uses the total items populated (i) as the end boundary
	std::sort(chunkOffsets.begin(), chunkOffsets.begin() + i, [](const ChunkPos& a, const ChunkPos& b) {
		// Calculate squared distance (avoids expensive square root calculations)
		int distSqA = (a.x * a.x) + (a.y * a.y) + (a.z * a.z);
		int distSqB = (b.x * b.x) + (b.y * b.y) + (b.z * b.z);

		return distSqA < distSqB;
		});
}

Chunk* WorldChunks::generateChunk(const ChunkPos& pos) {
	return generator->generateChunk(pos.x, pos.y, pos.z);
}

std::optional<ChunkPos> WorldChunks::getNextChunkToLoad(const glm::vec3& playerChunkPos) {
	for (unsigned int i = 0; i < RENDER_DISTANCE_VOLUME; i++) {
		// check if not already in loaded chunks
		ChunkPos candidatePos = {
			playerChunkPos.x + chunkOffsets[i].x,
			playerChunkPos.y + chunkOffsets[i].y,
			playerChunkPos.z + chunkOffsets[i].z
		};

		if (loadedChunks.find(candidatePos) != loadedChunks.end()) continue;
		if (pendingChunks.find(candidatePos) != pendingChunks.end()) continue;

		return candidatePos;
	}
	return std::nullopt;
}

void WorldChunks::render(Camera* camera) {
	for (const ChunkPos& pos : loadedChunks) {
		chunkMeshesMap[pos]->render(camera);
	}
}