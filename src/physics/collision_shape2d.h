#pragma once

#include "../common.h"
#include "../component.h"

class collision_shape2d : public component
{
public:
    vec2 size;
    vec2 offset;

    f32 density = 1.0f;
    f32 friction = 0.5f;
    f32 restitution = 0.0f;
    f32 restitution_threshold = 0.5f;
    u16 category = 0x0001;
    u16 mask = 0xFFFF;

    b32 trigger = false;
};