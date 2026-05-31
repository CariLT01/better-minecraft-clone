#include "WorldGeneratorScheduler.h"

WorldGeneratorScheduler::WorldGeneratorScheduler(size_t numThreads) : generator(new TerrainGenerator()) {
	for (size_t i = 0; i < numThreads; i++) {
		workers.emplace_back(&WorldGeneratorScheduler::queueLoop, this, i);
	}
}

void WorldGeneratorScheduler::queueLoop(int threadId) {
	while (true) {
		WorldGenTask itemToProcess;
		{
			std::unique_lock<std::mutex> lock(queueMutex);

			cv.wait(lock, [this]() {
				return !queue.empty();
				});

			itemToProcess = std::move(queue.front());
			queue.pop();
		}

		Chunk* generatedChunk = generateChunk(itemToProcess.x, itemToProcess.y, itemToProcess.z);

		WorldGenTaskResult res = {
			generatedChunk,
			itemToProcess.x, itemToProcess.y, itemToProcess.z
		};

		{
			std::unique_lock<std::mutex> lock(resultsMutex);

			results.push_back(std::move(res));
		}
	}
}

Chunk* WorldGeneratorScheduler::generateChunk(int x, int y, int z) {
	return generator->generateChunk(x, y, z);
}

void WorldGeneratorScheduler::addTask(const WorldGenTask& task) {
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		queue.push(task);
	}

	cv.notify_one();
}

void WorldGeneratorScheduler::clearResults() {
	results.clear();
}

std::vector<WorldGenTaskResult> WorldGeneratorScheduler::getResults() {
	return results;
}