#pragma once

#include "common.h"
#include "math.h"
#include "component.h"
#include "graphics/texture.h"

class sprite_component : public component
{
public:
    texture2d texture;
    vec4 color;
    vec2 size;
};

class transform_component : public component
{
public:
    vec3 position;
    vec3 rotation;
};
