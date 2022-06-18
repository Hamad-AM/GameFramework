#pragma once

#include "window.h"

#include "common.h"
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "application.h"

namespace atl
{
    class glfw_window : public window
    {
    public:
        void initialize(u32 width, u32 height, const char *title) override;
        b32 handle_input() override;
        b32 is_gamepad_connected() override;
        void update() override;
        void close() override;
    private:
        GLFWwindow* window;
        double previous_time;
        application game_;
    };
}