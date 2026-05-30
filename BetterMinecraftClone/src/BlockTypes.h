#pragma once
#include <vector>
#include <unordered_set>
#include <string>
#include <array>
#include <unordered_map>

struct BlockType {
	const char* topTexture;
	const char* sideTexture;
	const char* bottomTexture;
};

struct RuntimeBlockType {
	unsigned int topTextureIndex;
	unsigned int sideTextureIndex;
	unsigned int bottomTextureIndex;
};

constexpr unsigned int NUM_BLOCK_TYPES = 3;

constexpr BlockType blockTypes[NUM_BLOCK_TYPES] = {
	{
		"textures/grass_top.png",
		"textures/grass_side.png",
		"textures/dirt.png"
	},
	{
		"textures/dirt.png",
		"textures/dirt.png",
		"textures/dirt.png"
	},
	{
		"textures/stone.png",
		"textures/stone.png",
		"textures/stone.png"
	}
};

// Evaluate all at compile-time
// Unique textures, texture index and list resolvable during runtime

struct CompileTimeResult {
	std::array<std::string, NUM_BLOCK_TYPES * 3> uniqueTextures;
	std::array<RuntimeBlockType, NUM_BLOCK_TYPES> runtimeBlockTypes;
};

constexpr CompileTimeResult getRuntimeBlockTypes() {

	std::vector<std::string_view> already_seen;
	std::array<RuntimeBlockType, NUM_BLOCK_TYPES> runtimeBlockTypes;

	for (const BlockType& blockType : blockTypes) {
		
		auto itTop = std::lower_bound(already_seen.begin(), already_seen.end(), blockType.topTexture);
		if (itTop == already_seen.end() || *itTop != blockType.topTexture) {
			already_seen.insert(itTop, blockType.topTexture);
		}

		auto itSide = std::lower_bound(already_seen.begin(), already_seen.end(), blockType.sideTexture);
		if (itSide == already_seen.end() || *itSide != blockType.sideTexture) {
			already_seen.insert(itSide, blockType.sideTexture);
		}

		auto itBottom = std::lower_bound(already_seen.begin(), already_seen.end(), blockType.bottomTexture);
		if (itBottom == already_seen.end() || *itBottom != blockType.bottomTexture) {
			already_seen.insert(itBottom, blockType.bottomTexture);
		}

	}

	// Build runtime block types
	unsigned int i = 0;
	for (const BlockType& blockType : blockTypes) {
		RuntimeBlockType runtimeBlockType;
		auto it = std::lower_bound(already_seen.begin(), already_seen.end(), blockType.topTexture);
		runtimeBlockType.topTextureIndex = std::distance(already_seen.begin(), it);
		it = std::lower_bound(already_seen.begin(), already_seen.end(), blockType.sideTexture);
		runtimeBlockType.sideTextureIndex = std::distance(already_seen.begin(), it);
		it = std::lower_bound(already_seen.begin(), already_seen.end(), blockType.bottomTexture);
		runtimeBlockType.bottomTextureIndex = std::distance(already_seen.begin(), it);
		runtimeBlockTypes[i] = runtimeBlockType;
		i++;
	}

	// Convert already_seen to array
	std::array<std::string, NUM_BLOCK_TYPES * 3> uniqueTextures;
	for (size_t i = 0; i < already_seen.size(); i++) {
		uniqueTextures[i] = std::string(already_seen[i]);
	}
	
	return {uniqueTextures, runtimeBlockTypes};

}