#pragma once

#ifndef NDEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

constexpr unsigned int CHUNK_WIDTH = 32;
constexpr unsigned int CHUNK_HEIGHT = 256;
constexpr unsigned int CHUNK_AREA = CHUNK_WIDTH * CHUNK_WIDTH;
constexpr unsigned int CHUNK_VOLUME = CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT;


constexpr unsigned int SECTION_SIZE = 32;
constexpr unsigned int SECTION_VOLUME = SECTION_SIZE * SECTION_SIZE * SECTION_SIZE;
constexpr unsigned int SECTION_COUNT = CHUNK_HEIGHT / SECTION_SIZE;

constexpr unsigned int RENDER_DISTANCE = 6 + 1; // padding 1 because it waits on neighbor chunks

constexpr unsigned int TEXTURE_W = 16;
constexpr unsigned int TEXTURE_H = 16;

constexpr unsigned int THREAD_COUNT = 8;