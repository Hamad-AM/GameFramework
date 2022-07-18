#pragma once

#include "common.h"
#include "camera.h"
#include "physics/physics_system.h"
#include "graphics/renderer2d.h"
#include "audio.h"

#include <array>

namespace atl
{
    class entity;

    struct game_state
    {
        std::vector<ref<entity>> entities;

        physics_system physics;
        audio_system audio;
        
        renderer render;
        camera2d camera;
        vec3 camera_position;
    };
}