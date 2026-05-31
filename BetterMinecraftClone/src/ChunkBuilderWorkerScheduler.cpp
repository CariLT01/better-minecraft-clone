#include "ChunkBuilderWorkerScheduler.h"
#include <iostream>

ChunkBuilderWorkerScheduler::ChunkBuilderWorkerScheduler(const size_t threadCount) : chunkBuilder(new ChunkBuilder()) {
	for (size_t i = 0; i < threadCount; i++) {

		workers.emplace_back(&ChunkBuilderWorkerScheduler::queueLoop, this, i);
	}
}

void ChunkBuilderWorkerScheduler::queueLoop(int threadId) {
	while (true) {
		QueuedTask itemToProcess;
		{
			std::unique_lock<std::mutex> lock(queueMutex);

			cv.wait(lock, [this]() {
				return !queuedTasks.empty();
				});

			itemToProcess = std::move(queuedTasks.front());
			queuedTasks.pop();
		}

		Result res = buildChunk(itemToProcess);

		{
			std::unique_lock<std::mutex> lock(tasksMutex);

			finishedTasks.push_back(res);
		}
	}
}

void ChunkBuilderWorkerScheduler::addQueue(QueuedTask task) {
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		queuedTasks.push(std::move(task));

		if (queuedTasks.size() > 50) {
			std::cout << "Warning: there are " << queuedTasks.size() << " items queued to be processed! Is chunk builder overloaded?" << std::endl;
		}
	}

	cv.notify_one();
}

Result ChunkBuilderWorkerScheduler::buildChunk(const QueuedTask& task) {
	std::vector<Vertex> vertices = chunkBuilder->buildChunkMeshData(*task.input);
	return {
		vertices,
		task.x, task.y, task.z
	};
}

std::vector<Result> ChunkBuilderWorkerScheduler::getResults() {

	if (finishedTasks.size() > 50) {
		std::cout << "Warning: there are " << finishedTasks.size() << " results waiting to be processed" << std::endl;
	}

	return finishedTasks;
}

void ChunkBuilderWorkerScheduler::clearResults() {
	finishedTasks.clear();
}