#include "ChunkSectionView.h"
#include "MathUtils.h"
#include <iostream>
#include <cassert>

ChunkSectionView::ChunkSectionView(std::shared_ptr<Chunk> chunk, unsigned int sectionIndex) : chunkPtr(std::weak_ptr<Chunk>(chunk)),
begin(sectionIndex * CHUNK_AREA * SECTION_SIZE) {

	if (sectionIndex > SECTION_COUNT) {
		assert("Section index out of bounds: " + sectionIndex);
	}

	// std::cout << "Begin should be: " << begin << " section index: " << sectionIndex << std::endl;
}

ChunkSectionView::~ChunkSectionView() {

}

std::vector<ChunkSectionViewBuildData> ChunkSectionView::createChunkSectionViewsFromChunk(const ChunkPos& pos, std::shared_ptr<Chunk> chunk) {
	std::vector<ChunkSectionViewBuildData> views{};
	views.reserve(SECTION_COUNT);

	for (unsigned int i = 0; i < SECTION_COUNT; i++) {

		auto view = std::make_shared<ChunkSectionView>(chunk, i);
		views.push_back({
			SectionPos{pos.x, static_cast<int>(i), pos.z},
			view
		});

		// std::cout << "Section view at: " << i << std::endl;
	}

	return views;
}

uint8_t ChunkSectionView::getBlockAt(int x, int y, int z) {
	unsigned int local = getSectionIndex(x, y, z);
	unsigned int globalAccess = begin + local;

	std::shared_ptr<Chunk> chunkAcc = chunkPtr.lock();

	if (chunkAcc == nullptr) {
		// std::cout << "Warning: cannot query section block. Original chunk pointer has expired." << std::endl;
		assert("chunk pointer expired");
		return 0;
	}

	uint8_t block = chunkAcc->getBlockAt(globalAccess);;
	if (block != 0) {
		// std::cout << "we found something" << std::endl;
	}
	else {
		// std::cout << "Found nothing at: " << globalAccess << std::endl;
	}

	return block;
}