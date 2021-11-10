#include "glfw_window.h"

#include "platform.h"

#include <stdio.h>
#include <assert.h>

static void error_callback(s32 error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, s32 key, s32 scancode, s32 action, s32 mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void GLFWWindow::InitWindow(u32 width, u32 height, const char *title)
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        assert(true);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        assert(true);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);
}

b32 GLFWWindow::HandleInput()
{
    return b32();
}

b32 GLFWWindow::IsGamepadConnected()
{
    return b32();
}

void GLFWWindow::UpdateWindow()
{
    while (!glfwWindowShouldClose(window))
    {
        game_time = (float) glfwGetTime();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void GLFWWindow::CloseWindow()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main()
{
    GLFWWindow window;

    window.InitWindow(1280, 720, "Bobinon's great video game.");
    window.UpdateWindow();
    window.CloseWindow();
}