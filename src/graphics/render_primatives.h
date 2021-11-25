#pragma once

#include "../common.h"

struct vertex_p
{
    glm::vec3 position;
};

struct vertex_pnu
{
    glm::vec3 position;
    glm::vec3 normal;
    f32 uv[2];
};

struct vertex_pdsuukw
{
    glm::vec3 position;
    glm::vec4 diffuse_color;
    glm::vec4 specular_color;
    f32 uv0[2];
    f32 uv1[2];
    u8 join_indices[4];
    f32 weights[3];
};