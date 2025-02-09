#pragma once

#include "common.h"

struct MemoryArena
{
    size_t size{ 0 };
    size_t used{ 0 };
    u8* base{ nullptr };
};

MemoryArena InitArena(u8* start, size_t size);

#define PushStruct(arena, s) (s*)PushSize(arena, sizeof(s))
u8* PushSize(MemoryArena* arena, size_t size);

void ResetMemoryArena(MemoryArena* arena);
