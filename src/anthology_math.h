#ifndef ANTHOLOGY_MATH_H
#define ANTHOLOGY_MATH_H

#include "anthology_types.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef glm::vec2 v2;
typedef glm::vec3 v3;
typedef glm::vec4 v4;
typedef glm::mat4 m4;

inline float32 dot(v3 a, v3 b)
{
    return glm::dot(a, b);
}

inline float32 dot(v4 a, v4 b)
{
    return glm::dot(a, b);
}

#endif