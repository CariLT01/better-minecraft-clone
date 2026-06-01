#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <memory>
#include "ChunkMesh.h"
#include "ChunkBuilder.h"
 
struct Result {  
	std::vector<Vertex> vertices;
	int x;
	int y;
	int z;
};

struct QueuedTask {
	std::unique_ptr<ChunkDataInput> input;
	int x;
	int y;
	int z;
};

class ChunkBuilderWorkerScheduler {
public:
	ChunkBuilderWorkerScheduler(const size_t threadCount);
	~ChunkBuilderWorkerScheduler();

	void addQueue(QueuedTask task);
	std::vector<Result> getResults();
	void clearResults();

    std::mutex& getFinishedTasksMutex() {
       return tasksMutex;
    }
private:

	std::vector<std::thread> workers;
	std::vector<Result> finishedTasks;
	std::mutex tasksMutex;

	std::queue<QueuedTask> queuedTasks;
	std::mutex queueMutex;

	Result buildChunk(const QueuedTask& task);
	void queueLoop(int threadId);

	std::condition_variable cv;

	std::shared_ptr<ChunkBuilder> chunkBuilder;

	std::atomic<bool> stopRequested;

};