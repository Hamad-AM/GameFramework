#include "anthology.h"


static void initialize_arena(MemoryArena* arena, u64 size, u8* base_ptr)
{
    arena->max_size = size;
    arena->head = base_ptr;
    arena->size = 0;
}

#define push_struct(arena, type) (type*)push_size_(arena, sizeof(type))
#define push_array(arena, count, type) (type*)push_size_(arena, (count)*sizeof(type))
#define push_size(arena, size) push_size_(arena, size);
static void* push_size_(MemoryArena* arena, u32 size)
{
    void* struct_ptr = NULL;

    if ((arena->size + size) < arena->max_size)
    {
        struct_ptr = arena->head + arena->size;
        arena->size += size;
    }

    return struct_ptr;
}

static void game_update_and_render(RenderGroup* render_group, Memory* game_memory, Input* game_input)
{
    GameState* state = (GameState*)game_memory->permanent_storage;
    TransientState* tran_state = (TransientState*)game_memory->transient_storage;

    if (!game_memory->is_initialized)
    {
        game_memory->is_initialized = true;
        game_memory->top_ptr = game_memory->permanent_storage;

        initialize_arena(&tran_state->tran_arena,
                        game_memory->transient_storage_size,
                        (u8*)(game_memory->transient_storage));

        render_group->push_buffer_head = (u8*)push_size(&tran_state->tran_arena, render_group->max_push_buffer_size);
    }
}