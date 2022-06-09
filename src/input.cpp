#include "input.h"

namespace alg
{
    input* input::s_instance = new input(); 

    input::input()
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
    input::is_key_down_impl(key key)
    {
        return keyboard_.keys[key] == input_state::down;
    }

    b32
    input::is_key_up_impl(key key)
    {
        return keyboard_.keys[key] == input_state::up;
    }

    void
    input::set_key_state_impl(key key, input_state state)
    {
        keyboard_.keys[key] = state;
    }

    b32 
    input::is_left_mouse_down_impl()
    {
        return mouse_.left_button == input_state::down;
    }

    b32 
    input::is_left_mouse_pressed_impl()
    {
        return false;
    }

    b32 
    input::is_left_mouse_released_impl()
    {
        return false;
    }

    b32 
    input::is_left_mouse_up_impl()
    {
        return mouse_.left_button == input_state::up;
    }

    b32 
    input::is_right_mouse_down_impl()
    {
        return mouse_.right_button == input_state::down;
    }

    b32 
    input::is_right_mouse_pressed_impl()
    {
        return false;
    }

    b32 
    input::is_right_mouse_released_impl()
    {
        return false;
    }

    b32 
    input::is_right_mouse_up_impl()
    {
        return mouse_.right_button == input_state::up;
    }

    vec2
    input::mouse_position_impl()
    {
        return vec2{mouse_.x, mouse_.y};
    }


    void
    input::set_right_mouse_impl(input_state state)
    {
        mouse_.right_button = state;
    }

    void
    input::set_left_mouse_impl(input_state state)
    {
        mouse_.left_button = state;
    }

    void
    input::set_mouse_position_impl(s32 x, s32 y)
    {
        mouse_.x = x;
        mouse_.y = y;
    }
}