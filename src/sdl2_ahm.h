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
};

struct WindowHandle
{
    uint32 width;
    uint32 height;

    SDL_Window* window;
    SDL_GLContext context;
};