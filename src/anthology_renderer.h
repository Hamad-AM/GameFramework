#ifndef ANTHOLOGY_RENDERER_H
#define ANTHOLOGY_RENDERER_H

#include <string>
#include <vector>

#include <glad/glad.h>
#include <stb_image.h>


#include "anthology_math.h"
#include "anthology_types.h"

#include "anthology_opengl.h"


struct Vertex
{
    v3 position;
    v3 normal;
    v2 tex_coords;
};

struct Mesh
{
    std::vector<Vertex> vertices;
    uint64 number_of_vertices;

    std::vector<uint32> indices;
    uint64 number_of_indices;

    std::vector<Texture2D> textures;
    uint64 number_of_textures;
    
    VertexBuffer vb;
    VertexArray va;
    IndexBuffer ib;

    // TODO: need to consider mutiple textures
    Material material;
};

struct Model
{
    std::vector<Mesh> meshes;
    uint32 number_of_meshes;
};

void draw_texture_cube(TextureCube* cube, PerspectiveCamera* camera, Environment* env);

void draw_cube(Cube* cube, PerspectiveCamera* camera);

void update_camera(PerspectiveCamera* camera);

void init_cube(Cube* cube);

void init_texture_cube(TextureCube* cube);

void free_loaded_texture(LoadedTexture2D* loaded_texture);

LoadedTexture2D load_texture(const char* file_path, TextureFilter filter, TextureWrap wrapping);
#endif