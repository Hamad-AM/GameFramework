#ifndef ANTHOLOGY_H
#define ANTHOLOGY_H

#include "sdl2_anthology.h"
#include "anthology_renderer.h"

// Services that the platform layer provides to the game

struct Memory
{
    void* top_ptr;

    bool is_initialized;
    u64 permanent_storage_size;
    void* permanent_storage;

    u64 transient_storage_size;
    void* transient_storage;
};

struct GameState
{
    u32 player_health;
    Model* test_model;
};

struct MemoryArena
{
    u64 max_size;
    u64 size;
    u8* head;
};

struct TransientState
{
    bool is_initialized;
    MemoryArena tran_arena;
};

static Model load_model(Memory* game_memory, const char* path);

static void game_update_and_render(RenderGroup* render_group, Memory* game_memory, Input* game_input);

#endif