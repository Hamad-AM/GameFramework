#ifndef SDL_ANTHOLOGY_H
#define SDL_ANTHOLOGY_H

#include "platform.h"
#include "input.h"

class Window
{
public:
    virtual void
    InitWindow(u32 width, u32 height, const char *title) = 0;

    virtual b32
    HandleInput(Input* input) = 0;

    virtual b32
    IsGamepadConnected(void) = 0;

    virtual void
    UpdateWindow(void) = 0;

    virtual void
    CloseWindow(void) = 0;

    u32
    height(void) { return height_; }

    void
    set_height(u32 height) { height_ = height; }

    u32
    width(void) { return width_; }

    void
    set_width(u32 width) { width_ = width; }

protected:
    u32 height_;
    u32 width_;

private:
};

#endif