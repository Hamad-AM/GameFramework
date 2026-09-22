#pragma once

#include "common.h"

struct MemoryArena
{
    b32 initialize{ false };
    u64 size{ 0 };
    u64 used{ 0 };
    u8* base{ nullptr };
};

MemoryArena InitArena(u8* start, u64 size);

#define PushArray(arena, s, c) (s*)PushSize(arena, sizeof(s)*c)
#define PushStruct(arena, s) (s*)PushSize(arena, sizeof(s))
u8* PushSize(MemoryArena* arena, u64 size);

void ResetMemoryArena(MemoryArena* arena);
