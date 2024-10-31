#pragma once

#include "common.h"

#define PI 3.14159265358979323846264338327950288

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace atl
{
    typedef glm::vec2 vec2;
    typedef glm::vec3 vec3;
    typedef glm::vec4 vec4;
    typedef glm::mat4 mat4;

    typedef glm::ivec2 ivec2;
    typedef glm::ivec3 ivec3;

    inline mat4 frustum(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
    {
        return glm::frustum(left, right, bottom, top, near, far);
    }

    inline mat4 look_at(vec3& eye, vec3& center, vec3& up)
    {
        return glm::lookAt(eye, center, up);
    }

    inline mat4 ortho(f32 left, f32 right, f32 bottom, f32 top, f32 z_near, f32 z_far)
    {
        return glm::ortho(left, right, bottom, top, z_near, z_far);
    }

    inline mat4 perspective(f32 fov_y, f32 aspect, f32 near, f32 far)
    {
        return glm::perspective(fov_y, aspect, near, far);
    }

    inline mat4 scale(mat4& m, vec3& v)
    {
        return glm::scale(m, v);
    }

    inline mat4 rotate(mat4& m, f32 angle, vec3& axis)
    {
        return glm::rotate(m, angle, axis);
    }

    inline mat4 translate(mat4& m, vec3& v)
    {
        return glm::translate(m, v);
    }

    inline vec3 project(vec3& obj, mat4& model, mat4& proj, vec4& view_port)
    {
        return glm::project(obj, model, proj, view_port);
    }

    inline mat4 inverse(mat4& m)
    {
        return glm::inverse(m);
    }

    inline f32 determinant(mat4& m)
    {
        return glm::determinant(m);
    }

    inline mat4 transpose(mat4& m)
    {
        return glm::transpose(m);
    }

    inline f32 deg2rad(f32 degrees)
    {
        return glm::radians(degrees);
    }

    inline f32 rad2deg(f32 radians)
    {
        return glm::degrees(radians);
    }
}
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