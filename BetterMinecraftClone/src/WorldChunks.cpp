#include "WorldChunks.h"
#include "MathUtils.h"
#include "BlockTypes.h"

#include <iostream>
#include <algorithm>
#include <optional>

WorldChunks::WorldChunks(std::shared_ptr<ShaderProgram> terrainShaderProgram, std::shared_ptr<TextureArray> textureAtlas) : chunkMap(), loadedChunks(), pendingChunks(), chunkOffsets(), chunkMeshesMap(),
terrainShaderProgram(terrainShaderProgram), textureAtlas(textureAtlas), numTextures(getRuntimeBlockTypes().uniqueTextures.size()),
scheduler(new ChunkBuilderWorkerScheduler(THREAD_COUNT)), worldGenScheduler(new WorldGeneratorScheduler(THREAD_COUNT)) {
	precomputeChunkOffsets();
	shouldBeLoadedLookup.reserve(RENDER_DISTANCE_VOLUME);

}  

WorldChunks::~WorldChunks() {

}


void WorldChunks::update(glm::vec3 playerPosition) {
	glm::vec3 chunkPosition = realPositionToChunkPosition(playerPosition);

	// Loop through chunks that are ready

	{
		std::unique_lock<std::mutex> lock(worldGenScheduler->getResultsMutex());
		for (const WorldGenTaskResult& res : worldGenScheduler->getResults()) {
			std::shared_ptr<Chunk> newChunk = res.chunk;
			ChunkPos currentCpos = { res.x, res.y, res.z };

			chunkMap[currentCpos] = newChunk;

			generatingChunks.erase(currentCpos);

			// Add it to pending if not already in it
			if (pendingChunks.find(currentCpos) == pendingChunks.end()) {
				remeshChunk(currentCpos);
			}
		}

		worldGenScheduler->clearResults();
	}

	unsigned int queueSize;

	{
		std::unique_lock<std::mutex> lock(worldGenScheduler->getResultsMutex());
		queueSize = worldGenScheduler->getQueueSize();
	}
	int available = static_cast<int>(THREAD_COUNT) - static_cast<int>(queueSize);

	while (available > 0) {
		std::optional<ChunkPos> nextChunkToLoad = getNextChunkToLoad(chunkPosition);


		if (!nextChunkToLoad.has_value()) {
			// std::cout << "No more chunks to load" << std::endl;
			break;
		}

		// Generate chunk if it doesn't exist
		if (chunkMap.find(nextChunkToLoad.value()) == chunkMap.end()) {

			ChunkPos pos = nextChunkToLoad.value();

			worldGenScheduler->addTask({
				pos.x, pos.y, pos.z
				});

			generatingChunks.insert(pos);
			available -= 1;
		}
	}


	std::unordered_set<ChunkPos, ChunkPosHash> chunksToRemove;

	// Process already pending chunks
	for (const ChunkPos& pendingPos : pendingChunks) {
		if (isReadyToBuild(pendingPos)) {
			buildChunkMesh(pendingPos);
			loadingChunks.insert(pendingPos);
			chunksToRemove.insert(pendingPos);
		}
	}

	for (const ChunkPos& pos : chunksToRemove) {
		pendingChunks.erase(pos);
	}

	chunksToRemove.clear();


	// Handle chunks that have finished building
	{
		std::unique_lock<std::mutex> lock(scheduler->getFinishedTasksMutex());
		for (const Result& taskResult : scheduler->getResults()) {
			ChunkPos currentPos = { taskResult.x, taskResult.y, taskResult.z };

			if (taskResult.vertices.size() <= 0) {
				loadingChunks.erase(currentPos);
				loadedChunks.insert(currentPos);
				continue;
			}

			// check if chunk already exists
			if (chunkMeshesMap.find(currentPos) != chunkMeshesMap.end()) {
				// delete it
				chunkMeshesMap.erase(currentPos);
			}

			std::shared_ptr<ChunkMesh> chunkMesh = std::make_shared<ChunkMesh>(taskResult.vertices, std::vector<unsigned int>{}, RenderingContext{ terrainShaderProgram, textureAtlas, numTextures }, glm::vec3(taskResult.x, taskResult.y, taskResult.z) * glm::vec3(CHUNK_SIZE));
			chunkMeshesMap[currentPos] = chunkMesh;
			loadingChunks.erase(currentPos);
			loadedChunks.insert(currentPos);
		}
		scheduler->clearResults();
	}


	// Handle unloading

	// Build a fast lookup of should be currently loaded chunks
	
	shouldBeLoadedLookup.clear();

	for (unsigned int i = 0; i < RENDER_DISTANCE_VOLUME; i++) {
		ChunkPos candidatePos = {
			chunkPosition.x + chunkOffsets[i].x,
			chunkPosition.y + chunkOffsets[i].y,
			chunkPosition.z + chunkOffsets[i].z
		};
		shouldBeLoadedLookup.insert(candidatePos);
	}

	for (auto it = chunkMap.begin(); it != chunkMap.end(); ) {
		ChunkPos pos = it->first;
		// check if it's in chunk offsets
		if (shouldBeLoadedLookup.find(pos) == shouldBeLoadedLookup.end()) {
			// Unload it
			chunkMeshesMap.erase(pos);
			loadingChunks.erase(pos);
			pendingChunks.erase(pos);
			generatingChunks.erase(pos);
			loadedChunks.erase(pos);
			it = chunkMap.erase(it);
		}
		else {
			++it;
		}
	}


	
}

void WorldChunks::buildChunkMesh(const ChunkPos& pos) {
	std::shared_ptr<Chunk> chunkData = chunkMap[pos];

	std::unique_ptr<ChunkDataInput> chunkDataIn = std::make_unique<ChunkDataInput>(ChunkDataInput{
		.chunkData = chunkData,
		.front = chunkMap[{pos.x, pos.y, pos.z + 1}],
		.back = chunkMap[{pos.x, pos.y, pos.z - 1}],
		.left = chunkMap[{pos.x - 1, pos.y, pos.z}],
		.right = chunkMap[{pos.x + 1, pos.y, pos.z}],
		.top = chunkMap[{pos.x, pos.y + 1, pos.z}],
		.bottom = chunkMap[{pos.x, pos.y - 1, pos.z}]
	});

	scheduler->addQueue({
		std::move(chunkDataIn),
		pos.x, pos.y, pos.z
	});
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
		if (loadingChunks.find(candidatePos) != loadingChunks.end()) continue;
		if (generatingChunks.find(candidatePos) != generatingChunks.end()) continue;

		return candidatePos;
	}
	return std::nullopt;
}

void WorldChunks::render(std::shared_ptr < Camera> camera) {

	for (const auto& [pos, mesh] : chunkMeshesMap) {
		mesh->render(camera);
	}
}

void WorldChunks::remeshChunk(const ChunkPos& pos) {
	if (chunkMap.find(pos) == chunkMap.end()) {
		return;
	}
	pendingChunks.insert(pos);
}

uint8_t WorldChunks::getBlockAt(int x, int y, int z) {
	glm::vec3 cpos = realPositionToChunkPosition(glm::vec3(x, y, z));
	ChunkPos cposStruct = { cpos.x, cpos.y, cpos.z };

	if (chunkMap.find(cposStruct) == chunkMap.end()) {
		return 0;
	}

	std::shared_ptr<Chunk> c = chunkMap[cposStruct];

	int lx = (x % CHUNK_SIZE + CHUNK_SIZE) % CHUNK_SIZE;
	int ly = (y % CHUNK_SIZE + CHUNK_SIZE) % CHUNK_SIZE;
	int lz = (z % CHUNK_SIZE + CHUNK_SIZE) % CHUNK_SIZE;

	return c->getBlockAt(getIndex(lx, ly, lz));
}



void WorldChunks::setBlockAt(int x, int y, int z, uint8_t blockType) {
	glm::vec3 cpos = realPositionToChunkPosition(glm::vec3(x, y, z));
	ChunkPos cposStruct = { cpos.x, cpos.y, cpos.z };

	if (chunkMap.find(cposStruct) == chunkMap.end()) {
		return;
	}

	std::shared_ptr<Chunk> c = chunkMap[cposStruct];

	int lx = (x % CHUNK_SIZE + CHUNK_SIZE) % CHUNK_SIZE;
	int ly = (y % CHUNK_SIZE + CHUNK_SIZE) % CHUNK_SIZE;
	int lz = (z % CHUNK_SIZE + CHUNK_SIZE) % CHUNK_SIZE;

	c->setBlockAt(getIndex(lx, ly, lz), blockType);

	remeshModified(cposStruct, lx, ly, lz);
}

void WorldChunks::remeshModified(const ChunkPos& pos, int x, int y, int z) {
	std::vector<ChunkPos> toRemesh = { {0, 0, 0} };
	
	if (x == 0) {
		// remesh negX
		toRemesh.push_back({ -1, 0, 0 });
	}
	if (y == 0) {
		// remesh negY
		toRemesh.push_back({ 0, -1, 0 });
	}

	if (z == 0) {
		toRemesh.push_back({ 0, 0, -1 });
	}

	if (x == CHUNK_SIZE - 1) {
		toRemesh.push_back({ 1, 0, 0 });
	}

	if (y == CHUNK_SIZE - 1) {
		toRemesh.push_back({ 0, 1, 0 });
	}

	if (z == CHUNK_SIZE - 1) {
		toRemesh.push_back({ 0, 0, 1 });
	}

	for (const ChunkPos& toRemeshCoord : toRemesh) {
		int cx = pos.x + toRemeshCoord.x;
		int cy = pos.y + toRemeshCoord.y;
		int cz = pos.z + toRemeshCoord.z;

		remeshChunk({ cx, cy, cz });
	}
}