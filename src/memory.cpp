#include "memory.h"

MemoryArena InitArena(u8* start, u64 size)
{
    MemoryArena arena{};
    arena.size = size;
    arena.used = 0;
    arena.base = start;
    arena.initialize = true;
    return arena;
}

u8* PushSize(MemoryArena* arena, u64 size)
{
    assert(arena->initialize);
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
    assert(arena->initialize);
    arena->used = 0;
}
