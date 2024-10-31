#pragma once

#include "window.h"

#include "common.h"
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace atl
{
    class glfw_window : public platform_window
    {
    public:
        void initialize(u32 width, u32 height, const char *title) override;
        b32 handle_input() override;
        b32 is_gamepad_connected() override;
        void close() override;
        f64 get_time() override;
        void swap_buffers() override;
        b32 should_close() override;
    private:
        GLFWwindow* window;
        f64 previous_time;
    };
}