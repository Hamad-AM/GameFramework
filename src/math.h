#ifndef ANTHOLOGY_MATH_H
#define ANTHOLOGY_MATH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "platform.h"

typedef glm::vec2 v2;
typedef glm::vec3 v3;
typedef glm::vec4 v4;
typedef glm::mat4 m4x4;

inline f32 dot(v3 a, v3 b)
{
    return glm::dot(a, b);
}

inline f32 dot(v4 a, v4 b)
{
    return glm::dot(a, b);
}

inline m4x4 orthographic(f32 left, f32 right, f32 bottom, f32 top)
{
    return glm::ortho(left, right, bottom, top);
}

inline m4x4 orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 z_near, f32 z_far)
{
    return glm::ortho(left, right, bottom, top, z_near, z_far);
}

inline m4x4 perspective(f32 fov, f32 aspect, f32 z_near, f32 z_far)
{
    return glm::perspective(fov, aspect, z_near, z_far);
}

inline v2 v3_xy(v3 vec3)
{
    return v2(vec3.x, vec3.y);
}

inline m4x4 translate(v3 position)
{
    return glm::translate(glm::mat4(1.0f), position);
}

inline m4x4 translate(m4x4 mat, v3 position)
{
    return glm::translate(mat, position);
}

inline m4x4 scale(m4x4 mat, v3 scale_by)
{
    return glm::scale(mat, scale_by);
}

#endif