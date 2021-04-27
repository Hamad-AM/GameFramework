#ifndef SDL_ANTHOLOGY_H
#define SDL_ANTHOLOGY_H



#include "platform.h"
#include "input.h"

class Window
{
public:
    virtual void
    InitWindow(int width, int height, const char *title) = 0;

    virtual b32
    HandleInput(Input* input) = 0;

    virtual b32
    IsGamepadConnected(void) = 0;

    virtual void
    UpdateWindow(void) = 0;

    virtual void
    CloseWindow(void) = 0;

private:
};

#endif