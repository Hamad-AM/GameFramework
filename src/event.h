#pragma once

#include <functional>
#include <unordered_map>

#include "common.h"

namespace atl
{
    enum event_type
    {
        left_mouse_button_press, right_mouse_button_press,
        left_mouse_button_release, right_mouse_button_relase, mouse_move, key_press, key_release, window_close, window_resize, collision_enter
    };

    class event
    {
    public:
        event(event_type t) : type(t) {}

        b32 handled = false;
        event_type type;
    };

    typedef std::function<void(event*)> event_callback;
    using callback_functions = std::vector<event_callback>;

    class event_system
    {
    public:
        event_system()
        {}

        static event_system& get()
        { 
            static event_system s_es;
            return s_es;
        }

        void subscribe(event_type type, event_callback callback);

        void post_all(event_type type, event* event);

    private:
        std::unordered_map< event_type, callback_functions> observers;

        static event_system* g_event_system;
    };
}
