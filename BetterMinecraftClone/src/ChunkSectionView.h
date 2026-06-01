#pragma once

#include "Chunk.h"
#include <memory>
#include <vector>
#include "Types.h"


class ChunkSectionView;

struct ChunkSectionViewBuildData {
	SectionPos position;
	std::shared_ptr<ChunkSectionView> view;
};

class ChunkSectionView {
public:
	ChunkSectionView(std::shared_ptr<Chunk> chunkView, unsigned int sectionIndex);
	~ChunkSectionView();

	uint8_t getBlockAt(int x, int y, int z);

	static std::vector<ChunkSectionViewBuildData> createChunkSectionViewsFromChunk(const ChunkPos& pos, std::shared_ptr<Chunk> chunk);
private:
	std::weak_ptr<Chunk> chunkPtr;

	unsigned int begin;
};