#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdint.h>

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef unsigned char  ubyte;

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