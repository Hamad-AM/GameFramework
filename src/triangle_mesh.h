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

struct vertex_pnuvtb
{
    vec3 p;
    vec3 n;
    f32  uv[2];
    vec3 tan; // tangent
    vec3 bi_t;
    u8   bone[MAX_BONE_INFLUENCE];
    f32  w[MAX_BONE_INFLUENCE];
};

class triangle_mesh
{
public:
    triangle_mesh(std::vector<vertex_pnuvtb> vertices, std::vector<u32> indices, std::vector<texture2d> textures)
        : _vertices(vertices), _indices(indices), _textures(textures), _vertex_array(vertex_array), _initalized_render(false)
    {

    }

    void initialize();

    std::vector<vertex_pnuvtb> get_vertex() { return _vertices; }
    std::vector<u32>           get_indices() { return _indices; }
    std::vector<texture2d>     get_textures() { return _textures; }
    b32                        get_initialized() { return _initalized_render; }
    u32                        get_vertex_array() { return _vertex_array; }

private:
    std::vector<vertex_pnuvtb> _vertices;
    std::vector<u32>           _indices;
    std::vector<texture2d>     _textures;
    u32 _vertex_array;
    u32 _vertex_buffer;
    u32 _index_buffer;
    b32 _initalized_render;
};