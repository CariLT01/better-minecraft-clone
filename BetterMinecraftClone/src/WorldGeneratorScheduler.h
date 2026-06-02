#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <condition_variable>
#include "Chunk.h"
#include "TerrainGenerator.h"
#include "Types.h"
#include "ThreadPool.h"

struct WorldGenTask {
	ChunkPos pos;
};

struct WorldGenTaskResult {
	std::shared_ptr<Chunk> chunk;
	ChunkPos pos;
};

class WorldGeneratorScheduler {
public:
	WorldGeneratorScheduler(size_t numThreads);
	~WorldGeneratorScheduler();

	void addTask(const WorldGenTask& task);

	std::mutex& getResultsMutex() {
		return resultsMutex;
	}

	std::vector<WorldGenTaskResult> getResults();
	void clearResults();

	unsigned int getQueueSize() {
		return queue.size();
	}

private:
	std::vector<std::thread> workers;
	std::mutex queueMutex;

	std::queue<WorldGenTask> queue;
	std::vector<WorldGenTaskResult> results;
	std::mutex resultsMutex;

	void queueLoop(int threadId);

	std::condition_variable cv;

	std::shared_ptr<Chunk> generateChunk(int x, int z);

	std::shared_ptr<TerrainGenerator> generator;

	std::atomic<bool> stopRequested{ false };

	std::shared_ptr<ThreadPool> threadPool;


};