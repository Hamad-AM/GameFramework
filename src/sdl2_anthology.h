#ifndef SDL2_ANTHOLOGY_H
#define SDL2_ANTHOLOGY_H

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <glad/glad.h>
#include <stb_image.h>

#include "anthology_types.h"
#include "anthology_renderer.h"

struct WindowHandle
{
    uint32 width;
    uint32 height;

    SDL_Window* window;
    SDL_GLContext context;
};

#endif