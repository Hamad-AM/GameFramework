#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdint.h>

#include <glad/glad.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef unsigned char  ubyte;

typedef glm::vec3 v3;
typedef glm::vec4 v4;

struct Material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

struct LightMaterial
{
    glm::vec3 position;
    glm::vec3 color;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
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
    glm::vec3 position;
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 projection_view;
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
    LoadedTexture2D* loaded_texture;
};


enum ShaderType
{
    SHADER_FLOAT,
};

struct BufferLayoutAttrib
{
    ShaderType type;
    uint32 count;
    bool normalized;
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

struct WindowHandle
{
    uint32 width;
    uint32 height;

    SDL_Window* window;
    SDL_GLContext context;
};

struct TextureCube
{
    float vertices[288];
    VertexBuffer vb;
    VertexArray va;
    Shader shader;
    glm::vec3 position;
    glm::mat4 rotation;
    Texture2D texture;
    Material material;
};

struct Cube
{
    float vertices[180];
    VertexBuffer vb;
    VertexArray va;
    Shader shader;
    glm::vec3 position;
    glm::mat4 rotation;
};