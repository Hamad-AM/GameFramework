#ifndef SDL_WINDOW_HH
#define SDL_WINDOW_HH

#include "window.h"

#define SDL_MAIN_HANDLED
#include <SDL.h>

class SDLWindow : public Window
{
public:
    virtual void
        InitWindow(int width, int height, char* title) override;

    virtual b32
        HandleInput(Input* input) override;

    virtual b32
        IsGamepadConnected(void) override;

    virtual void
        UpdateWindow(void) override;

    virtual void
        CloseWindow(void) override;

private:
    u32 height_;
    u32 width_;

    SDL_Window* window_;
    SDL_GLContext context_;

    SDL_Event event_;

};

#endif