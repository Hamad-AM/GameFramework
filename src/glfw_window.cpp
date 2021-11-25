#include "glfw_window.h"

#include <stdio.h>
#include <assert.h>

#include <iostream>

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

void glfw_window::initialize(u32 width, u32 height, const char *title)
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        assert(true);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        assert(true);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        assert(false);
    }    
    glfwSwapInterval(1);

    game_.initialize();
}

b32 glfw_window::handle_input()
{
    return b32();
}

b32 glfw_window::is_gamepad_connected()
{
    return b32();
}

void glfw_window::update()
{
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        game_time = (float) glfwGetTime();

        glfwPollEvents();
        game_.update(game_time);
        game_.draw(game_time);
        glfwSwapBuffers(window);

    }
}

void glfw_window::close()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main()
{
    glfw_window window;

    window.initialize(1280, 720, "Bobinon's great video game.");
    window.update();
    window.close();
}