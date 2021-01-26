#include "anthology.h"

static void game_update_and_render(Memory* game_memory, Input* game_input)
{
    GameState* state = (GameState*)game_memory->permanent_storage;

    if (!game_memory->is_initialized)
    {
        game_memory->is_initialized = true;
        game_memory->top_ptr = game_memory->permanent_storage;
    }
}