#include "ThreadPool.h"
#include "Chunk.h"

struct LightingTask {
	std::shared_ptr<Chunk> chunk;
	std::vector<LightStack> initialLightStack;
	ChunkPos chunkPos;
};

struct LightingTaskResult {
	ChunkPos chunkPos;
	std::unordered_map<ChunkPos, std::vector<LightStack>, ChunkPosHash> lightBorder;
};

class LightingScheduler {
public:
	LightingScheduler();
	~LightingScheduler();

	void addTask(const LightingTask& task);
	std::vector<LightingTaskResult> getResults() {
		return results;
	}

	std::mutex& getResultsMutex() {
		return resultsMutex;
	}

	void clearResults() {
		results.clear();
	}
private:

	std::shared_ptr<ThreadPool> threadPool;
	std::vector<LightingTaskResult> results;

	std::mutex resultsMutex;


};