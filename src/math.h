#pragma once

#include "common.h"

#define PI 3.14159265358979323846264338327950288

#include <glm/glm.hpp>

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;

struct ivec2
{
    s32 x, y;
};

struct ivec3
{
    s32 x, y, z;
};



// inline f64 sin(f63 rad)
// {
//     const s32 max_terms = 10;
//     return rad * sine_series(rad, 0, max_terms);
// }

// inline f64 sine_series(f64 rad, s32 i, s32 max_terms)
// {
//     if (i > max_terms)
//         return 1.0;
//     return 1.0 - (rad * rad / (2.0 * i + 2.0) / (2.0 * i + 3.0) * sine_series(rad, i + 1, max_terms) );
// }

// inline f64 cos(f64 rad)
// {
//     return sin((PI / 2) - rad);
// }