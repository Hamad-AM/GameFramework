#ifndef SDL2_ANTHOLOGY_H
#define SDL2_ANTHOLOGY_H

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <glad/glad.h>
#include <stb_image.h>

#include "anthology_renderer.h"

#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float f32;
typedef double f64;

typedef unsigned char ubyte;

struct WindowHandle
{
    u32 width;
    u32 height;

    SDL_Window* window;
    SDL_GLContext context;
};

#endif