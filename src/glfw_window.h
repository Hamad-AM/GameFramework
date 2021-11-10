#ifndef GLFW_WINDOW
#define GLFW_WINDOW

#include "window.h"

#include "platform.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

class GLFWWindow : public Window
{
public:
    void InitWindow(u32 width, u32 height, const char *title) override;
    b32 HandleInput() override;
    b32 IsGamepadConnected() override;
    void UpdateWindow() override;
    void CloseWindow() override;
private:
    GLFWwindow* window;
    f32 game_time;
};

#endif