#pragma once

//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include <stb_image.h>

#include "common.h"
#include "asset_system.h"
#include "graphics/renderer2d.h"
#include "camera.h"
#include "input.h"
#include "entity.h"
#include "glfw_window.h"


#include <glm/glm.hpp>


#include <iostream>
#include <vector>
#include <unordered_map>
#include "audio/mini_audio.h"
#include "render.h"

struct GameState
{
    
};

class application
{
public:
    application();
    ~application();

    void initialize(u32 screen_width, u32 screen_height);
    void run();
    void update(f32 dt);
    void close();
    void UploadDataToGL(MeshRenderData& data);

private:
    MemoryArena permanent_storage;
    MemoryArena transient_storage;

    u32 screen_width_;
    u32 screen_height_;
    f64 previous_time;
    platform_window* window;

    mini_audio_system audio;

    renderer render;
    RenderData renderData;

    camera2d camera2d;
    vec3 camera2d_position;
    Camera3D camera;

    glm::vec3 lightPosition;
    std::unordered_map<std::string, u32> gpu_textures;
    std::vector<MeshRenderData> batchMeshRenderData;
    f32 current_position;
    glm::vec2 lastMousePos;
    f32 pitch = 0;
    f32 yaw = 0;

    AssetSystem assets;
};
