#pragma once

#include "common.h"
#include "math.h"
#include "graphics/texture.h"

struct vertex_1p
{
    vec3 p;
};

struct vertex_1p1n1uv
{
    vec3 p;
    vec2 n;
    f32  uv[2]; // [u, v] texture coordinates
};

struct vertex_1p1d1s2uv4j
{
    vec3 p;
    vec4 d;
    vec4 s;
    f32 uv0[2];
    f32 uv1[2];
    u8  k[4];
    f32 w[3];
};

#define MAX_BONE_INFLUENCE 4

struct vertex
{
    vec3 p;
    vec3 n;
    f32  uv[2];
    vec3 tan; // tangent
    vec3 bi_t;
    u8   bone[MAX_BONE_INFLUENCE];
    f32  w[MAX_BONE_INFLUENCE];
};

struct triangle_mesh
{
    std::vector<vertex> vertices;
    std::vector<u32> indices;
    std::vector<texture2d> textures;
    u32 vertex_array;
    u32 vertex_buffer;
    u32 index_buffer;
    b32 initalized_render;
};

struct model
{
    std::vector<texture2d> textures;
    std::vector<triangle_mesh> meshes;
    const char* path;
};

model load_model(const char* path)
{
    model m;

    return m;
}