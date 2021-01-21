#ifndef ANTHOLOGY_OPENGL_H
#define ANTHOLOGY_OPENGL_H

#include <glad/glad.h>

#include "anthology_types.h"
#include "anthology_math.h"

enum ShaderType
{
    SHADER_FLOAT = 0,
    SHADER_INT,
};

struct PointLight
{
    v3 position;
    v3 color;

    float constant;
    float linear;
    float quadratic;

    v3 ambient;
    v3 diffuse;
    v3 specular;
};

struct DirectionalLight
{
    v3 direction;
    v3 ambient;
    v3 diffuse;
    v3 specular;
};

struct SpotLight
{
    v3 position;
    v3 direction;
    float innerCutOff;
    float outerCutOff;

    v3 ambient;
    v3 diffuse;
    v3 specular;
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

struct PerspectiveCamera
{
    v3 position;
    m4 view;
    m4 projection;
    m4 projection_view;
    float fov;
    // TODO: Rotation
};

struct OrthographicCamera
{

};

struct LoadedTexture2D
{
    ubyte* image_data;
    int32 width;
    int32 height;
    int32 number_of_channels;
    TextureWrap wrap;
    TextureFilter filter;
};

struct Texture2D
{
    uint32 id;
    uint32 slot;
    LoadedTexture2D* loaded_texture;
};

struct BufferLayoutAttrib
{
    ShaderType type;
    uint32 count;
    bool normalized;
};

struct Material
{
    Texture2D diffuse;
    Texture2D specular;
    float shininess;
};

struct VertexBuffer
{
    uint32 id; 
    // TODO: Somekind of buffer layout?
};

struct IndexBuffer
{
    uint32 id;
    uint32 count;
};

struct VertexArray
{
    uint32 id;
    VertexBuffer* vertex_buffer;
    IndexBuffer* index_buffer;
};

struct Shader
{
    uint32 program;
};

struct TextureCube
{
    float vertices[288];
    VertexBuffer vb;
    VertexArray va;
    Shader shader;
    v3 position;
    m4 rotation;
    Material material;
};

struct Cube
{
    float vertices[180];
    VertexBuffer vb;
    VertexArray va;
    Shader shader;
    v3 position;
    m4 rotation;
};

static const uint32 NO_POINT_LIGHTS = 4;

struct Environment
{
    DirectionalLight dir_light;
    PointLight point_lights[NO_POINT_LIGHTS];
    SpotLight spot_light;
};

struct Scene2D
{

};

struct Mesh
{
    float32* vertices;
    uint32 number_of_vertices;
    uint32* indices;
    uint32 number_of_indices;
    Texture2D* textures;
    uint32 number_of_texture_slots;
};

struct Scene
{
    Environment env;
    Mesh meshes[256];
};

void bind_index_buffer(IndexBuffer* index_buffer);

void create_index_buffer(IndexBuffer* index_buffer, uint32* indices, uint32 count);

void unbind_index_buffer();

void delete_index_buffer(IndexBuffer* index_buffer);

void bind_vertex_buffer(VertexBuffer* vertex_buffer);

void create_vertex_buffer(VertexBuffer* vertex_buffer, float* vertices, uint32 size);

void unbind_vertex_buffer();

void delete_vertex_buffer(VertexBuffer* vertex_buffer);

void bind_vertex_array(VertexArray* vertex_array);

void delete_vertex_array(VertexArray* vertex_array);

void unbind_vertex_array(VertexArray* vertex_array);

uint32 shader_type_size(ShaderType type);

GLenum convert_shadertype_to_gltype(ShaderType type);

void assign_vertex_attrib_layout(BufferLayoutAttrib* layout, uint32 number_of_attrib);

void create_vertex_array(VertexArray* vertex_array);

void bind_shader(Shader* shader);

void unbind_shader(Shader* shader);

void delete_shader(Shader* shader);

void compile_shader(Shader* shader, const char* fragment_src, const char* vertex_src);

void load_shader(Shader* shader, const char* file_path);

void upload_uniform_bool(const char* name, bool value, Shader* shader);

void upload_uniform_int32(const char* name, int32 value, Shader* shader);

void upload_uniform_float(const char* name, float value, Shader* shader);

void upload_uniform_mat4(const char* name, glm::mat4 mat4, Shader* shader);

void upload_uniform_vec3(const char* name, glm::vec3 vec3, Shader* shader);

void upload_uniform_vec4(const char* name, glm::vec4 vec4, Shader* shader);

void clear_screen();

GLenum convert_to_gl_filter(TextureFilter filter);

GLenum convert_to_gl_wrap(TextureWrap wrap);

LoadedTexture2D load_texture(const char* file_path, TextureFilter filter, TextureWrap wrapping);

void free_loaded_texture(LoadedTexture2D* loaded_texture);

void create_texture(Texture2D* texture, LoadedTexture2D* loaded_texture);

void bind_texture(Texture2D* texture);

void unbind_texture(Texture2D* texture);

void delete_texture(Texture2D* texture);

void upload_material_uniform(Material* material, Shader* shader);

void upload_spot_light_uniform(SpotLight* light, Shader* shader);

void upload_directional_light_uniform(DirectionalLight* light, Shader* shader);

void upload_point_light_uniform(PointLight* light, int32 index, Shader* shader);

#endif