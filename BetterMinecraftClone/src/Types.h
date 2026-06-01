#pragma once

#include <functional>

struct ChunkPos {
    int x, z;

    // Fast equality operator
    bool operator==(const ChunkPos& other) const {
        return x == other.x && z == other.z;
    }
};

struct ChunkPosHash {
    size_t operator()(const ChunkPos& pos) const {
        size_t h1 = std::hash<int>{}(pos.x);
        size_t h3 = std::hash<int>{}(pos.z);
        // Standard bit-shifting to combine hashes
        return h1 ^ (h3 << 2);
    }
};

struct SectionPos {
    int x, y, z;

    bool operator==(const SectionPos& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

struct SectionPosHash {
    size_t operator()(const SectionPos& pos) const {
        size_t h1 = std::hash<int>{}(pos.x);
        size_t h2 = std::hash<int>{}(pos.y);
        size_t h3 = std::hash<int>{}(pos.z);

        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};