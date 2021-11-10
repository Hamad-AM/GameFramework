// #ifndef SDL_WINDOW_HH
// #define SDL_WINDOW_HH

// #include "window.h"

// #define SDL_MAIN_HANDLED
// #include <SDL.h>
// #include "game.h"

// class SDLWindow : public Window
// {
// public:
//     void
//     InitWindow(u32 width, u32 height, const char* title) override;

//     b32
//     HandleInput() override;

//     b32
//     IsGamepadConnected(void) override;

//     void
//     UpdateWindow(void) override;

//     void
//     CloseWindow(void) override;

// private:
//     SDL_Window* window_;
//     SDL_GLContext context_;

//     SDL_Event event_;
//     Game game;
// };

// #endif