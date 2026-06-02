#include "WorldGeneratorScheduler.h"
#include <iostream>

WorldGeneratorScheduler::WorldGeneratorScheduler(size_t numThreads) : generator(std::make_shared<TerrainGenerator>()), threadPool(std::make_shared<ThreadPool>(8)) {

}

WorldGeneratorScheduler::~WorldGeneratorScheduler() {
	
}


std::shared_ptr<Chunk> WorldGeneratorScheduler::generateChunk(int x, int z) {
	return generator->generateChunk(x, z);
}

void WorldGeneratorScheduler::addTask(const WorldGenTask& task) {
	auto future = threadPool->enqueue([this, task]() {
		std::shared_ptr<Chunk> chunk = generateChunk(task.pos.x, task.pos.z);
		WorldGenTaskResult result{ chunk, task.pos };
		{
			std::unique_lock<std::mutex> lock(resultsMutex);
			results.push_back(result);
		}
	});
}

void WorldGeneratorScheduler::clearResults() {
	results.clear();
}

std::vector<WorldGenTaskResult> WorldGeneratorScheduler::getResults() {
	return results;
}