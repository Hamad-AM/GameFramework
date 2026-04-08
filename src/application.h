#pragma once

//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include <stb_image.h>

#include "common.h"
#include "asset_system.h"
#include "camera.h"

#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "audio/mini_audio.h"
#include "render.h"
#include "window.h"

struct GameState
{
#define MAX_LIGHTS 10
    Light lights[MAX_LIGHTS];
    u32 numberOfLights{ 0 };
};

class application
{
public:
    application();
    ~application();

    void initialize(u32 screen_width, u32 screen_height);
    void run();
    void update(f32 dt, GameState* state);
    void close();

private:
    MemoryArena permanent_storage;
    MemoryArena transient_storage;

    u32 screen_width_;
    u32 screen_height_;
    f64 previous_time;
    platform_window* window;

    mini_audio_system audio;

    RenderData renderData;

    // camera2d camera2d;
    // vec3 camera2d_position;
    Camera3D camera;

    glm::vec3 lightPosition;
    f32 current_position;
    glm::vec2 lastMousePos;
    f32 pitch = 0;
    f32 yaw = 0;

    AssetSystem assets;
};
