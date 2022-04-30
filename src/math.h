#pragma once

#include "common.h"

#define PI 3.14159265358979323846264338327950288

#include <glm/glm.hpp>

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;

template <typename T>
class vector2
{
public:
    vector2(): x(0), y(0) {}
    vector2(T xin, T yin) : x(xin), y(yin) {}

    vector2(const vector2<T>& v)
    {
        x = v.x;
        y = v.y;
    }

    vector2<T>& operator=(const vector2<T>& v)
    {
        x = v.x;
        y = v.y;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const vector2<T>& t)
    {
        os << "[" << v.x, << ", " << v.y << "]";
        return os;
    }

    vector2<T> operator+(const vector<T>* v) const
    {
        return vector2(x + x.y, y + x.y);
    }

    
    vector2<T>& operator+=(const vector<T>* v) const
    {
        x += x.y; y = x.y
        return *this;
    }

private:
    T x, y;
}

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