#include "input.h"

input_system::input_system()
{
    for (s32 i = 0; i < 104; ++i)
    {
        keyboard_.keys[i] = {};
        keyboard_.keys[i] = input_state::up;
    }

    mouse_ = {};
    mouse_.left_button = input_state::up;
    mouse_.middle_button = input_state::up;
    mouse_.right_button = input_state::up;
    mouse_.scroll_wheel_value = 0;
    mouse_.x = 0;
    mouse_.y = 0;

    for (s32 i = 0; i < 4; ++i)
    {
        game_pad_[i] = {};
        game_pad_[i].A = input_state::up;
        game_pad_[i].B = input_state::up;
        game_pad_[i].back = input_state::up;
        game_pad_[i].left_shoulder = input_state::up;
        game_pad_[i].right_shoulder = input_state::up;
        game_pad_[i].start = input_state::up;
        game_pad_[i].X = input_state::up;
        game_pad_[i].Y = input_state::up;
        game_pad_[i].down = input_state::up;
        game_pad_[i].left = input_state::up;
        game_pad_[i].right = input_state::up;
        game_pad_[i].up = input_state::up;
        game_pad_[i].left_stick = input_state::up;
        game_pad_[i].right_stick = input_state::up;

        game_pad_[i].left_stick_x = 0.0f;
        game_pad_[i].left_stick_y = 0.0f;
        game_pad_[i].right_stick_x = 0.0f;
        game_pad_[i].right_stick_y = 0.0f;
        game_pad_[i].left_trigger = 0.0f;
        game_pad_[i].right_trigger = 0.0f;

    }
}

b32
input_system::is_key_down(key key)
{
    return keyboard_.keys[key] == input_state::down;
}

b32
input_system::is_key_up(key key)
{
    return keyboard_.keys[key] == input_state::up;
}

void
input_system::set_key_state(key key, input_state state)
{
    keyboard_.keys[key] = state;
}