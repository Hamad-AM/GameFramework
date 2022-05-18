#pragma once

#include "common.h"

#define PI 3.14159265358979323846264338327950288

#include <glm/glm.hpp>

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;

using ivec2 = glm::ivec2;
using ivec3 = glm::ivec3;

// class v2
// {

// };

// class v3
// {

// };

// v2 cross(v2& a, v2& b);
// v2 dot(v2& a, v2& b);

// v3 cross(v3& a, v3& b);
// v3 dot(v3& a, v3& b);

// mat4 translate(mat4& m, v3& translation);
// mat4 scale(mat4& m);
// mat4 rotate();
// mat4 determinant(mat4& m);
// mat4 transpose();

// class mat4
// {

// };

// template<typename T>
// void* get_math_ptr(T m);



// class v2
// {
// public:
//     v2(): x(0), y(0) {}
//     v2(f32 xin, f32 yin) : x(xin), y(yin) {}

//     v2(const v2& v)
//     {
//         x = v.x;
//         y = v.y;
//     }

//     v2& operator=(const v2& v)
//     {
//         x = v.x;
//         y = v.y;
//         return *this;
//     }

//     friend std::ostream& operator<<(std::ostream& os, const v2& t)
//     {
//         os << "[" << v.x, << ", " << v.y << "]";
//         return os;
//     }

//     v2 operator+(const v2& v) const
//     {
//         return v2(x + v.y, y + v.y);
//     }

    
//     v2& operator+=(const v2& v) const
//     {
//         x += v.x;
//         y += v.y;
//         return *this;
//     }

// private:
//     f32 x, y;
// }

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