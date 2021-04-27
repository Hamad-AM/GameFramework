#ifndef ANTHOLOGY_RENDERER_H
#define ANTHOLOGY_RENDERER_H

#include <string>
#include <vector>

#include <glad/glad.h>
#include <stb_image.h>


#include "anthology_math.h"
#include "sdl2_anthology.h"

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
    u64 number_of_vertices;

    std::vector<u32> indices;
    u64 number_of_indices;

    std::vector<Texture2D> textures;
    u64 number_of_textures;
    
    VertexBuffer vb;
    VertexArray va;
    IndexBuffer ib;

    // TODO: need to consider mutiple textures
    Material material;
};

struct Model
{
    std::vector<Mesh> meshes;
    u32 number_of_meshes;
    Shader* shader;
};

struct RenderGroup
{
    PerspectiveCamera camera;

    DirectionalLight dir_light;
    PointLight point_lights[4];
    SpotLight spot_light;

    u32 max_push_buffer_size;
    u32 push_buffer_size;
    u8* push_buffer_head;
};

struct RenderGroup2D
{
    OrthographicCamera camera;
};

struct RenderTexture
{
    v2 position;
    Texture2D* texture;
};

struct RenderModel
{
    v2 position;
    Model* model;
};

inline void* push_render_element(RenderGroup* group, u32 size)
{
    void* element = 0;

    if ((group->push_buffer_size + size) < group->max_push_buffer_size)
    {
        element = group->push_buffer_head + group->push_buffer_size;
        group->push_buffer_size += size;
    }
    else 
    {
        assert(true);
    }

    return element;
}

inline void push_texture(RenderGroup* group,  Texture2D* texture, const v2* position)
{
    RenderTexture* entity = (RenderTexture*)push_render_element(group, sizeof(RenderTexture));
    entity->position = *position;
    entity->texture = texture;
}

inline void push_model(RenderGroup* group, Model* model, v2* position)
{
    RenderModel* entity = (RenderModel*)push_render_element(group, sizeof(RenderModel));
    entity->position = *position;
    entity->model = model;
}

void draw_texture_cube(TextureCube* cube, PerspectiveCamera* camera, Environment* env);

void draw_cube(Cube* cube, PerspectiveCamera* camera);

void update_camera(PerspectiveCamera* camera);

void init_cube(Cube* cube);

void init_texture_cube(TextureCube* cube);

void free_loaded_texture(LoadedTexture2D* loaded_texture);

LoadedTexture2D load_texture(const char* file_path, TextureFilter filter, TextureWrap wrapping);
#endif