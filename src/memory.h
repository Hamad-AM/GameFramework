#pragma once

#include "common.h"

struct MemoryArena
{
    size_t size;
    size_t used;
    u8* base;
};


#define PushStruct(arena, s) PushSize(arena, sizeof(s)
void* PushSize(MemoryArena* arena, size_t size)
{
    if (arena->used + size >= arena->size)
    {
        assert("No more memory");
        return nullptr;
    }
    else
    {
        void* result = arena->base + arena->used;
        arena->used += size;
        return result;
    }
}

void* Reset(MemoryArena* arena)
{
    arena->used = 0;
}
