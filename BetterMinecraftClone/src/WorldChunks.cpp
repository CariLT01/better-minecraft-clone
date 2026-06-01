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
	glm::ivec2 chunkPosition = realPositionToChunkPosition(playerPosition);

	// Loop through chunks that are ready

	{
		std::unique_lock<std::mutex> lock(worldGenScheduler->getResultsMutex());
		for (const WorldGenTaskResult& res : worldGenScheduler->getResults()) {
			std::shared_ptr<Chunk> newChunk = res.chunk;
			ChunkPos currentCpos = res.pos;

			chunkMap[currentCpos] = newChunk;

			generatingChunks.erase(currentCpos);

			// Add it to pending if not already in it
			std::vector<ChunkSectionViewBuildData> results = ChunkSectionView::createChunkSectionViewsFromChunk(currentCpos, newChunk);
			for (const auto& data : results) {
				chunkSections[data.position] = data.view;
				if (pendingChunks.find(data.position) == pendingChunks.end()) {
					// std::cout << "Query for remeshing" << std::endl;
					remeshChunk(data.position);
				}
				
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
		std::optional<ChunkPos> nextChunkToLoad = getNextChunkToLoad({chunkPosition.x, chunkPosition.y});


		if (!nextChunkToLoad.has_value()) {
			// std::cout << "No more chunks to load" << std::endl;
			break;
		}

		// Generate chunk if it doesn't exist
		if (chunkMap.find(nextChunkToLoad.value()) == chunkMap.end()) {

			ChunkPos pos = nextChunkToLoad.value();

			worldGenScheduler->addTask({
				pos.x, pos.z
				});

			generatingChunks.insert(pos);
			available -= 1;
		}
	}


	std::unordered_set<SectionPos, SectionPosHash> chunksToRemove;

	// Process already pending chunks
	for (const SectionPos& pendingPos : pendingChunks) {
		if (isReadyToBuild(pendingPos)) {
			buildChunkMesh(pendingPos);
			loadingChunks.insert(pendingPos);
			chunksToRemove.insert(pendingPos);
		}
	}

	for (const SectionPos& pos : chunksToRemove) {
		pendingChunks.erase(pos);
	}

	chunksToRemove.clear();


	// Handle chunks that have finished building
	{
		std::unique_lock<std::mutex> lock(scheduler->getFinishedTasksMutex());
		for (const Result& taskResult : scheduler->getResults()) {
			SectionPos currentPos = taskResult.pos;

			// std::cout << "Chunk has finished building" << std::endl;
			
			if (taskResult.vertices.size() <= 0) {
				// std::cout << "Found empty chunk" << std::endl;
				loadingChunks.erase(currentPos);
				loadedChunks.insert(currentPos);
				continue;
			}

			// check if chunk already exists
			if (chunkMeshesMap.find(currentPos) != chunkMeshesMap.end()) {
				// delete it
				chunkMeshesMap.erase(currentPos);
			}

			std::shared_ptr<ChunkMesh> chunkMesh = std::make_shared<ChunkMesh>(taskResult.vertices, std::vector<unsigned int>{}, RenderingContext{ terrainShaderProgram, textureAtlas, numTextures }, glm::vec3(taskResult.pos.x, taskResult.pos.y, taskResult.pos.z) * glm::vec3(SECTION_SIZE));
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
			chunkPosition.y + chunkOffsets[i].z
		};
		shouldBeLoadedLookup.insert(candidatePos);
	}

	for (auto it = chunkMap.begin(); it != chunkMap.end(); ) {
		ChunkPos pos = it->first;
		// check if it's in chunk offsets
		if (shouldBeLoadedLookup.find(pos) == shouldBeLoadedLookup.end()) {
			// Unload it

			for (unsigned int i = 0; i < SECTION_COUNT; i++) {

				SectionPos spos = { pos.x, i, pos.z };

				chunkMeshesMap.erase(spos);
				loadingChunks.erase(spos);
				pendingChunks.erase(spos);
				loadedChunks.erase(spos);
				chunkSections.erase(spos);
			}

			generatingChunks.erase(pos);

			it = chunkMap.erase(it);
		}
		else {
			++it;
		}
	}


	
}

void WorldChunks::buildChunkMesh(const SectionPos& pos) {
	std::shared_ptr<ChunkSectionView> chunkData = chunkSections[pos];

	std::unique_ptr<ChunkDataInput> chunkDataIn = std::make_unique<ChunkDataInput>(ChunkDataInput{
		.chunkData = chunkData,
		.front = chunkSections[{pos.x, pos.y, pos.z + 1}],
		.back = chunkSections[{pos.x, pos.y, pos.z - 1}],
		.left = chunkSections[{pos.x - 1, pos.y, pos.z}],
		.right = chunkSections[{pos.x + 1, pos.y, pos.z}],
		.top = chunkSections[{pos.x, pos.y + 1, pos.z}],
		.bottom = chunkSections[{pos.x, pos.y - 1, pos.z}]
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

bool WorldChunks::isReadyToBuild(const SectionPos& pos) {
	for (unsigned int i = 0; i < 6; i++) {
		if (i == 4 && pos.y == SECTION_COUNT - 1) {
			continue;
		}
		if (i == 5 && pos.y == 0) {
			continue;
		}
		SectionPos neighborPos = {
			pos.x + neighbors[i * 3 + 0],
			pos.y + neighbors[i * 3 + 1],
			pos.z + neighbors[i * 3 + 2]
		};
		if (chunkSections.find(neighborPos) == chunkSections.end()) return false;
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
				chunkOffsets[i] = { x, z };
				i++;
			}
		}
	}

	// 2. Sort the array so the closest chunks to (0,0,0) come first
	// This uses the total items populated (i) as the end boundary
	std::sort(chunkOffsets.begin(), chunkOffsets.begin() + i, [](const ChunkPos& a, const ChunkPos& b) {
		// Calculate squared distance (avoids expensive square root calculations)
		int distSqA = (a.x * a.x) + (a.z * a.z);
		int distSqB = (b.x * b.x) + (b.z * b.z);

		return distSqA < distSqB;
		});
}

std::optional<ChunkPos> WorldChunks::getNextChunkToLoad(const ChunkPos& playerChunkPos) {
	for (unsigned int i = 0; i < RENDER_DISTANCE_VOLUME; i++) {
		// check if not already in loaded chunks
		ChunkPos candidatePos = {
			playerChunkPos.x + chunkOffsets[i].x,
			playerChunkPos.z + chunkOffsets[i].z
		};

		if (generatingChunks.find(candidatePos) != generatingChunks.end()) continue;
		if (chunkMap.find(candidatePos) != chunkMap.end()) continue;

		return candidatePos;
	}
	return std::nullopt;
}

void WorldChunks::render(std::shared_ptr < Camera> camera) {

	for (const auto& [pos, mesh] : chunkMeshesMap) {
		mesh->render(camera);
	}
}

void WorldChunks::remeshChunk(const SectionPos& pos) {
	if (chunkSections.find(pos) == chunkSections.end()) {
		return;
	}
	pendingChunks.insert(pos);
}

uint8_t WorldChunks::getBlockAt(int x, int y, int z) {
	glm::ivec2 cpos = realPositionToChunkPosition(glm::vec3(x, y, z));
	ChunkPos cposStruct = { static_cast<int>(cpos.x), static_cast<int>(cpos.y) };

	if (chunkMap.find(cposStruct) == chunkMap.end()) {
		return 0;
	}

	std::shared_ptr<Chunk> c = chunkMap[cposStruct];

	int lx = (x % CHUNK_WIDTH + CHUNK_WIDTH) % CHUNK_WIDTH;
	int ly = y;
	int lz = (z % CHUNK_WIDTH + CHUNK_WIDTH) % CHUNK_WIDTH;

	if (ly < 0) {
		return 0;
	}

	return c->getBlockAt(getChunkIndex(lx, ly, lz));
}



void WorldChunks::setBlockAt(int x, int y, int z, uint8_t blockType) {
	glm::ivec2 cpos = realPositionToChunkPosition(glm::vec3(x, y, z));
	ChunkPos cposStruct = { static_cast<int>(cpos.x), static_cast<int>(cpos.y) };

	if (chunkMap.find(cposStruct) == chunkMap.end()) {
		return;
	}

	std::shared_ptr<Chunk> c = chunkMap[cposStruct];

	int lx = (x % CHUNK_WIDTH + CHUNK_WIDTH) % CHUNK_WIDTH;
	int ly = y;
	int lz = (z % CHUNK_WIDTH + CHUNK_WIDTH) % CHUNK_WIDTH;

	int sly = y / SECTION_SIZE;

	c->setBlockAt(getChunkIndex(lx, ly, lz), blockType);

	remeshModified({cpos.x, sly, cpos.y}, lx, ly, lz);
}

void WorldChunks::remeshModified(const SectionPos& pos, int x, int y, int z) {
	std::vector<SectionPos> toRemesh = { {0, 0, 0} };
	
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

	if (x == SECTION_SIZE - 1) {
		toRemesh.push_back({ 1, 0, 0 });
	}

	if (y == SECTION_SIZE - 1) {
		toRemesh.push_back({ 0, 1, 0 });
	}

	if (z == SECTION_SIZE - 1) {
		toRemesh.push_back({ 0, 0, 1 });
	}

	for (const SectionPos& toRemeshCoord : toRemesh) {
		int cx = pos.x + toRemeshCoord.x;
		int cy = pos.y + toRemeshCoord.y;
		int cz = pos.z + toRemeshCoord.z;

		remeshChunk({ cx, cy, cz });
	}
}