#pragma once

#include "common.h"
#include "gmath.h"
#include "graphics/texture.h"

namespace alg 
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

    class sprite_component : public component<sprite_component>
    {
    public:
        texture2d texture;
        vec4 color;
        vec2 size;

        void update(f32 dt) {}
    };

    class transform_component : public component<transform_component>
    {
    public:
        vec3 position;
        vec3 rotation;

        void update(f32 dt) {}
    };
}