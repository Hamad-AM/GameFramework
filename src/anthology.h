#ifndef ANTHOLOGY_H
#define ANTHOLOGY_H

#include "anthology_types.h"
#include "anthology_renderer.h"

// Services that the platform layer provides to the game

enum ButtonState
{
    Up,
    Down,
};

enum Keys
{
    a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,
    BACKSPACE, TAB, SPACE, RIGHT, LEFT, DOWN, UP, LCTRL, LSHIFT, LALT, ESCAPE,
};

struct GamePad
{
    ButtonState A;
    ButtonState B;
    ButtonState back;
    ButtonState left_shoulder;
    ButtonState right_shoulder;
    ButtonState start;
    ButtonState X;
    ButtonState Y;
    ButtonState down;
    ButtonState left;
    ButtonState right;
    ButtonState up;
    ButtonState left_stick;
    ButtonState right_stick;

    float left_stick_x;
    float left_stick_y;
    float right_stick_x;
    float right_stick_y;

    float left_trigger;
    float right_trigger;
};

struct Keyboard
{
    ButtonState keys[104];
};

struct Mouse
{
    ButtonState left_button;
    ButtonState right_button;
    ButtonState middle_button;
    int32 scroll_wheel_value;
    int32 x;
    int32 y;
};

struct Input
{
    bool game_pad_connected;
    GamePad game_pad[4];
    Keyboard keyboard;
    Mouse mouse;
};

struct Memory
{
    void* top_ptr;

    bool is_initialized;
    uint64 permanent_storage_size;
    void* permanent_storage;

    uint64 transient_storage_size;
    void* transient_storage;
};

struct GameState
{
    uint32 player_health;
    Model* test_model;
};

static Model load_model(Memory* game_memory, const char* path);

static void game_update_and_render(Memory* game_memory, Input* game_input);


#endif