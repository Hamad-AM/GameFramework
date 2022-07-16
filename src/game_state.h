#pragma once

#include "common.h"
#include "camera.h"
#include "physics/physics_system.h"
#include "graphics/renderer2d.h"

#include <array>

namespace atl
{
    class entity;

    struct game_state
    {
        std::vector<ref<entity>> entities;

        physics_system physics;
        
        renderer render;
        camera2d camera;
        vec3 camera_position;
    };
}