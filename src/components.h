#pragma once

#include "common.h"
#include "gmath.h"
#include "graphics/texture.h"

namespace atl 
{
    template<typename T>
    class component
    {
    public:
        void update(f32 dt)
        {
            static_cast<T*>(this)->update(dt); 
        }
        void destroy()
        {
            static_cast<T*>(this)->destroy();
        }
    };

    class sprite : public component<sprite>
    {
    public:
        texture2d texture;
        vec4 color;
        vec2 size;
        f32 tiling_factor;


        void update(f32 dt) {}

        void destroy() { return; }
    };

    class transform : public component<transform>
    {
    public:
        vec3 position;
        vec2 size;
        vec3 rotation;

        void update(f32 dt) {}

        void destroy() { return; }
    };
}