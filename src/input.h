#ifndef ANTHOLOGY_INPUT_H
#define ANTHOLOGY_INPUT_H

#include "common.h"

enum input_state
{
    up = 0,
    down,
};

enum key
{
    a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,
    BACKSPACE, TAB, SPACE, RIGHT, LEFT, DOWN, UP, LCTRL, LSHIFT, LALT, ESCAPE,
};


class input_system
{
public:
    input_system();

    struct GamePad
    {
        input_state A;
        input_state B;
        input_state back;
        input_state left_shoulder;
        input_state right_shoulder;
        input_state start;
        input_state X;
        input_state Y;
        input_state down;
        input_state left;
        input_state right;
        input_state up;
        input_state left_stick;
        input_state right_stick;

        float left_stick_x;
        float left_stick_y;
        float right_stick_x;
        float right_stick_y;

        float left_trigger;
        float right_trigger;
    };

    struct Keyboard
    {
        input_state keys[104];
    };

    struct Mouse
    {
        input_state left_button;
        input_state right_button;
        input_state middle_button;
        s32 scroll_wheel_value;
        s32 x;
        s32 y;
    };

    b32
    is_key_down(key key);
    
    b32
    is_key_up(key key);
    
    b32
    is_key_pressed(key key);

    void
    set_key_state(key key, input_state state);

    void
    SetLeftMouseBtn(input_state state);

    void
    SetRightMouseBtn(input_state state);

    void
    SetMiddleMouseBtn(input_state state);

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