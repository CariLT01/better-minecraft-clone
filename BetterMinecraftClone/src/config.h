#pragma once

#ifndef NDEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

constexpr unsigned int CHUNK_SIZE = 32;
constexpr unsigned int CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
constexpr unsigned int CHUNK_VOLUME = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

constexpr unsigned int RENDER_DISTANCE = 6;

constexpr unsigned int TEXTURE_W = 16;
constexpr unsigned int TEXTURE_H = 16;

constexpr unsigned int THREAD_COUNT = 8;