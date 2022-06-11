#pragma once

#include "common.h"
#include "graphics/renderer2d.h"
#include "graphics/texture.h"
#include "camera.h"
#include "physics/physics_system.h"
#include "input.h"
#include "entity.h"

#include <iostream>

namespace alg
{
    class application
    {
    public:
        application();
        ~application();

        void initialize(u32 screen_width, u32 screen_height);
        void update(f32 dt);
        void draw(f32 dt);
        void close();

    private:
        renderer render;

        texture2d test_sprite;

        camera2d camera;
        vec3 camera_position;

        u32 screen_width_;
        u32 screen_height_;

        physics_system physics_system_;
    };
}