#include "sdl_window.h"
#include "input.h"

#include "math.h"
#include "platform.h"

#include <windows.h>
#include <memoryapi.h>

struct Camera
{
    m4x4 projection;
    v3 position;
};
struct RenderGroup
{
    Camera pers_camera;
    Camera ortho_camera;

    u32 max_push_buffer_size;
    u32 push_buffer_size;
    u8* push_buffer_head;
};

struct Memory
{
    void* top_ptr;

    bool is_initialized;
    u64 permanent_storage_size;
    void* permanent_storage;

    u64 transient_storage_size;
    void* transient_storage;
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

int
main()
{
    SDLWindow window;
    window.InitWindow(1280, 720, "Game");

    Input input;

    Memory game_memory = {};
    game_memory.permanent_storage_size = 64 * 1024 * 1024;
    game_memory.permanent_storage = VirtualAlloc(0, game_memory.permanent_storage_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

    game_memory.transient_storage_size = 4294967296;
    game_memory.transient_storage = VirtualAlloc(0, game_memory.transient_storage_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

    RenderGroup render_group;

    render_group.ortho_camera = {};
    render_group.ortho_camera.projection = orthographic(0.0f, (f32)window.width(), (f32)window.height(), 0.0f, -1.0f, 1.0f);
    render_group.ortho_camera.position = v3(0, 0, 0);

    render_group.pers_camera = {};
    render_group.pers_camera.projection = perspective(0.0f, 0.0f, 0.0f, 0.0f);
    render_group.pers_camera.position = v3(0, 0, 0);

    render_group.max_push_buffer_size = 128 * 1024 * 1024;
    render_group.push_buffer_head = NULL;
    render_group.push_buffer_size = 0;

    b32 running = true;
    while (running)
    {
        running = window.HandleInput(&input);
        
        if (input.IsKeyDown(Keys::ESCAPE))
            running = false;
        
        window.UpdateWindow();
    }
    window.CloseWindow();

    VirtualFree((LPVOID)game_memory.permanent_storage, (SIZE_T)game_memory.permanent_storage_size, MEM_RELEASE);
    VirtualFree((LPVOID)game_memory.transient_storage, (SIZE_T)game_memory.transient_storage_size, MEM_RELEASE);
}