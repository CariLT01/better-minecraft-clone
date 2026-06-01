#include "WorldGeneratorScheduler.h"
#include <iostream>

WorldGeneratorScheduler::WorldGeneratorScheduler(size_t numThreads) : generator(std::make_shared<TerrainGenerator>()) {
	for (size_t i = 0; i < numThreads; i++) {
		workers.emplace_back(&WorldGeneratorScheduler::queueLoop, this, i);
	}
}

WorldGeneratorScheduler::~WorldGeneratorScheduler() {
	stopRequested = true;
	cv.notify_all();
	for (auto& worker : workers) {
		std::cout << "world gen workers: waiting for worker to join" << std::endl;
		worker.join();
	}
}

void WorldGeneratorScheduler::queueLoop(int threadId) {
	while (!stopRequested) {
		WorldGenTask itemToProcess;
		{
			std::unique_lock<std::mutex> lock(queueMutex);

			cv.wait(lock, [this]() {
				return !queue.empty() || stopRequested;
				});

			if (stopRequested) {
				break;
			}

			itemToProcess = std::move(queue.front());
			queue.pop();
		}

		std::shared_ptr<Chunk> generatedChunk = generateChunk(itemToProcess.x, itemToProcess.y, itemToProcess.z);

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

std::shared_ptr<Chunk> WorldGeneratorScheduler::generateChunk(int x, int y, int z) {
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