#include <vector>
#include <thread>
#include <queue>
#include <condition_variable>
#include "Chunk.h"
#include "TerrainGenerator.h"

struct WorldGenTask {
	int x;
	int y;
	int z;
};

struct WorldGenTaskResult {
	Chunk* chunk;
	int x;
	int y;
	int z;
};

class WorldGeneratorScheduler {
public:
	WorldGeneratorScheduler(size_t numThreads);

	void addTask(const WorldGenTask& task);

	std::mutex& getResultsMutex() {
		return resultsMutex;
	}

	std::vector<WorldGenTaskResult> getResults();
	void clearResults();

private:
	std::vector<std::thread> workers;
	std::mutex queueMutex;

	std::queue<WorldGenTask> queue;
	std::vector<WorldGenTaskResult> results;
	std::mutex resultsMutex;

	void queueLoop(int threadId);

	std::condition_variable cv;

	Chunk* generateChunk(int x, int y, int z);

	TerrainGenerator* generator;


};