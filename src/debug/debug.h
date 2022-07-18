#pragma once

#include "../common.h"
#include "../game_state.h"

#include "b2_debug_draw.h"

namespace atl
{
    struct settings
    {
        vec4 collision_shape_color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    };

    class debugger
    {
    public:
        void start();
        void end();

        void draw_collisions(const game_state& state);

        void flush();

        //DEBUG
        settings sett;

        box2d::B2DebugDraw b2_debug_draw;
    
    private:
    };
}