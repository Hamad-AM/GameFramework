#pragma once

#include "common.h"
#include "graphics/renderer2d.h"
#include "graphics/texture.h"
#include "camera.h"
#include "physics/physics_system.h"
#include "input.h"
#include "entity.h"
#include "glfw_window.h"
#include "game_state.h"
#include "debug/debug.h"
#include "audio.h"

#include "debug/b2_debug_draw.h"

#include "Ground.h"

#include <iostream>

namespace atl
{
    class application
    {
    public:
        application();
        ~application();

        void initialize(u32 screen_width, u32 screen_height);
        void run();
        void update(f32 dt);
        void close();

    private:
        game_state state;
        ref<sound> coin_sound;

        debugger dbg;

        u32 screen_width_;
        u32 screen_height_;
        f64 previous_time;
        platform_window* window;
    };
}