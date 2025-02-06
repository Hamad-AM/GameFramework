#pragma once

#include "common.h"
#include "input.h"

    class platform_window
    {
    public:
        virtual void
        initialize(u32 width, u32 height, const char *title) = 0;

        virtual b32
        handle_input() = 0;

        virtual b32
        is_gamepad_connected(void) = 0;
        
        virtual void
        close(void) = 0;

        virtual f64
        get_time() = 0;

        virtual void
        swap_buffers() = 0;

        virtual b32
        should_close() = 0;

        u32
        height(void) { return height_; }

        void
        set_height(u32 height) { height_ = height; }

        u32
        width(void) { return width_; }

        void
        set_width(u32 width) { width_ = width; }

    protected:
        u32 height_;
        u32 width_;
    };
