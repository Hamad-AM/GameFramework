#include "window.h"
#include "input.h"

#include "anthology_math.h"

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

int
main()
{
    Window window(1280, 720, "Game");
    window.InitWindow();

    Input input;

    Memory game_memory = {};
    game_memory.permanent_storage_size = 64 * 1024 * 1024;
    game_memory.permanent_storage = VirtualAlloc(0, game_memory.permanent_storage_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

    game_memory.transient_storage_size = 4294967296;
    game_memory.transient_storage = VirtualAlloc(0, game_memory.transient_storage_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

    RenderGroup render_group;

    render_group.ortho_camera = {};
    render_group.ortho_camera.projection = orthographic(0.0f, screen_width, screen_height, 0.0f, -1.0f, 1.0f);
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
}