#pragma once

#include "common.h"
#include "gmath.h"
#include "event.h"

namespace atl
{
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


    class mouse_event : public event
    {
    public:
        mouse_event(event_type t, vec2 position) : mouse_position(position), event(t) {}

        vec2 mouse_position;
    };

    class input
    {
    public:
        input();

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

        inline static b32
        is_key_down(key key) { return s_instance->is_key_down_impl(key); }
        
        inline static b32
        is_key_up(key key) { return s_instance->is_key_up_impl(key); }
        
        inline static b32
        is_key_pressed(key key) { return s_instance->is_key_pressed_impl(key); }

        inline static b32
        is_left_mouse_down() { return s_instance->is_left_mouse_down_impl(); }
        
        inline static b32
        is_left_mouse_up() { return s_instance->is_left_mouse_up_impl(); }
        
        inline static b32
        is_left_mouse_released() { return s_instance->is_left_mouse_released_impl(); }
        
        inline static b32
        is_left_mouse_pressed() { return s_instance->is_left_mouse_pressed_impl(); }

        inline static b32
        is_right_mouse_down() { return s_instance->is_right_mouse_down_impl(); }
        
        inline static b32
        is_right_mouse_up() { return s_instance->is_right_mouse_up_impl(); }

        inline static void
        set_key_state(key key, input_state state) { s_instance->set_key_state_impl(key, state); }

        inline static void
        set_right_mouse(input_state state) { s_instance->set_right_mouse_impl(state); }

        inline static void
        set_left_mouse(input_state state) { s_instance->set_left_mouse_impl(state); }

        inline static void
        set_mouse_position(s32 x, s32 y) { s_instance->set_mouse_position_impl(x, y); }

        inline static vec2
        mouse_position() { return s_instance->mouse_position_impl(); }

        Keyboard&
        keyboard(void) { return keyboard_; }
        
        GamePad*
        gamepad(void) { return game_pad_; }
        
        Mouse&
        mouse(void) { return mouse_; }

    private:
        b32
        is_key_down_impl(key key);
        
        b32
        is_key_up_impl(key key);
        
        b32
        is_key_pressed_impl(key key);

        void
        set_key_state_impl(key key, input_state state);

        b32 is_left_mouse_down_impl();
        b32 is_left_mouse_pressed_impl();
        b32 is_left_mouse_released_impl();
        b32 is_left_mouse_up_impl();

        b32 is_right_mouse_down_impl();
        b32 is_right_mouse_pressed_impl();
        b32 is_right_mouse_released_impl();
        b32 is_right_mouse_up_impl();

        void set_right_mouse_impl(input_state state);
        void set_left_mouse_impl(input_state state);
        void set_mouse_position_impl(s32 x, s32 y);

        vec2 mouse_position_impl();

    private:
        b32 game_pad_connected_;
        GamePad game_pad_[4];
        Keyboard keyboard_;
        Mouse mouse_;
        static input* s_instance;
    };
}