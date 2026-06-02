#include "LightingScheduler.h"

LightingScheduler::LightingScheduler() : threadPool(std::make_shared<ThreadPool>(8)) {

}

LightingScheduler::~LightingScheduler() {

}

void LightingScheduler::addTask(const LightingTask& task) {
	auto future = threadPool->enqueue([this, task]() {
		auto borderLight = task.chunk->calculateLight(task.initialLightStack, task.chunkPos.x, task.chunkPos.z);

		LightingTaskResult results;
		results.chunkPos = task.chunkPos;
		results.lightBorder = borderLight;
		{
			std::unique_lock<std::mutex> lock(resultsMutex);
			this->results.push_back(results);
		}
	});
}

