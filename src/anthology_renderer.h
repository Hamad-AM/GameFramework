#ifndef ANTHOLOGY_RENDERER_H
#define ANTHOLOGY_RENDERER_H

#include "anthology_math.h"
#include "anthology_opengl.h"

// TODO: Temporary includes need to remove and Mesh/Model classes

#include <vector>

struct Material
{
};

struct Shader
{

};

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
    
    u32 vb;
    u32 va;
    u32 ib;

    // TODO: need to consider mutiple textures
    Material material;
};

struct Model
{
    std::vector<Mesh> meshes;
    u32 number_of_meshes;
    Shader* shader;
};

enum TextureType
{
    DIFFUSE,
    SPECULAR,
    NORMAL,
    BUMP,
    METAL,
    AO,
    ROUGHNESS,
    HEIGHT,
};

enum TextureFilter
{
    NEAREST_NEIGHBOR,
    BILINEAR,
};

enum TextureWrap
{
    REPEAT,
    MIRRORED_REPEAT,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
};

struct Texture2D
{
    ubyte* image_data;
    s32 width;
    s32 height;
    s32 number_of_channels;
    const char* path;
    TextureWrap wrap;
    TextureFilter filter;
    TextureType type;
    u32 id;
    u32 slot;
    bool initialized;
};

struct Camera
{
    m4x4 projection;
    v3 position;
};

struct RenderGroup
{
    Camera pers_camera;
    Camera ortho_camera;

    u32 max_push_buffer_size;
    u32 push_buffer_size;
    u8* push_buffer_head;
};

enum RenderType
{
    RenderType_Texture,
    RenderType_Model,
    RenderType_Rectangle,
};

struct RenderElement
{
    v3 position;
    RenderType type;
    void* element;
};

inline void* push_render_element(RenderGroup* group)
{
    void* element = 0;

    u32 size = sizeof(RenderElement);

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

inline void push_texture(RenderGroup* group, Texture2D* texture, v3* position)
{
    RenderElement* render_element = (RenderElement*)push_render_element(group);
    render_element->position = *position;
    render_element->type = RenderType_Texture;
    render_element->element = (void*)texture;
}

inline void push_model(RenderGroup* group, Model* model, v3* position)
{
    RenderElement* render_element = (RenderElement*)push_render_element(group);
    render_element->position = *position;
    render_element->type = RenderType_Model;
    render_element->element = (void*)model;
}

inline void render_to_output(RenderGroup* group)
{
    gl_render_to_output(group);
}

inline void delete_texture(Texture2D* texture)
{
    gl_delete_texture(texture);
}

#endif