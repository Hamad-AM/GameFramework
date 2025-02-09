#include "memory.h"

MemoryArena InitArena(u8* start, size_t size)
{
    MemoryArena arena{};
    arena.size = size;
    arena.used = 0;
    arena.base = start;
    return arena;
}

u8* PushSize(MemoryArena* arena, size_t size)
{
    if (arena->used + size >= arena->size)
    {
        assert(false);
        return nullptr;
    }
    else
    {
        u8* result = arena->base + arena->used;
        arena->used += size;
        return result;
    }
}

void ResetMemoryArena(MemoryArena* arena)
{
   arena->used = 0;
}
