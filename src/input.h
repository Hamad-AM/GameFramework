#ifndef ANTHOLOGY_INPUT_H
#define ANTHOLOGY_INPUT_H

#include "common.h"

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
    s32 scroll_wheel_value;
    s32 x;
    s32 y;
};

class Input
{
public:
    Input();

    b32
    IsKeyDown(Keys key);
    
    b32
    IsKeyUp(Keys key);
    
    b32
    IsKeyPressed(Keys key);

    void
    SetKeyState(Keys key, ButtonState state);

    void
    SetLeftMouseBtn(ButtonState state);

    void
    SetRightMouseBtn(ButtonState state);

    void
    SetMiddleMouseBtn(ButtonState state);

    Keyboard&
    keyboard(void) { return keyboard_; }
    
    GamePad*
    gamepad(void) { return game_pad_; }
    
    Mouse&
    mouse(void) { return mouse_; }

private:
    b32 game_pad_connected_;
    GamePad game_pad_[4];
    Keyboard keyboard_;
    Mouse mouse_;

};

#endif