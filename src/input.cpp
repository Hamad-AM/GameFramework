#include "input.h"

Input::Input()
{
    for (s32 i = 0; i < 104; ++i)
    {
        keyboard_.keys[i] = {};
        keyboard_.keys[i] = ButtonState::Up;
    }

    mouse_ = {};
    mouse_.left_button = ButtonState::Up;
    mouse_.middle_button = ButtonState::Up;
    mouse_.right_button = ButtonState::Up;
    mouse_.scroll_wheel_value = 0;
    mouse_.x = 0;
    mouse_.y = 0;

    for (s32 i = 0; i < 4; ++i)
    {
        game_pad_[i] = {};
        game_pad_[i].A = ButtonState::Up;
        game_pad_[i].B = ButtonState::Up;
        game_pad_[i].back = ButtonState::Up;
        game_pad_[i].left_shoulder = ButtonState::Up;
        game_pad_[i].right_shoulder = ButtonState::Up;
        game_pad_[i].start = ButtonState::Up;
        game_pad_[i].X = ButtonState::Up;
        game_pad_[i].Y = ButtonState::Up;
        game_pad_[i].down = ButtonState::Up;
        game_pad_[i].left = ButtonState::Up;
        game_pad_[i].right = ButtonState::Up;
        game_pad_[i].up = ButtonState::Up;
        game_pad_[i].left_stick = ButtonState::Up;
        game_pad_[i].right_stick = ButtonState::Up;

        game_pad_[i].left_stick_x = 0.0f;
        game_pad_[i].left_stick_y = 0.0f;
        game_pad_[i].right_stick_x = 0.0f;
        game_pad_[i].right_stick_y = 0.0f;
        game_pad_[i].left_trigger = 0.0f;
        game_pad_[i].right_trigger = 0.0f;

    }
}

b32
Input::IsKeyDown(Keys key)
{
    return keyboard_.keys[key] == ButtonState::Down;
}

b32
Input::IsKeyUp(Keys key)
{
    return keyboard_.keys[key] == ButtonState::Up;
}