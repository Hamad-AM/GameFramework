#include "glfw_window.h"

#include <stdio.h>
#include <assert.h>

#include "input.h"

#include <iostream>

bool isExtensionSupported(const char* extension) {
    GLint numExtensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

    // Loop through all extensions and check if the specific one is present
    for (GLint i = 0; i < numExtensions; ++i) {
        const char* ext = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));
        if (ext && strcmp(ext, extension) == 0) {
            return true; // Extension found
        }
    }
    return false; // Extension not found
}

void GLAPIENTRY
MessageCallback( GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar* message,
                const void* userParam )
{
    if(type==GL_DEBUG_TYPE_ERROR)
    {
        fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
                type, severity, message );
    }
    // assert(type != GL_DEBUG_TYPE_ERROR);
}

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

static void mouse_callback(GLFWwindow* window, f64 xpos, f64 ypos)
{
    input::set_mouse_position(xpos, ypos);
}

void glfw_window::initialize(u32 width, u32 height, const char *title)
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        assert(true);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        assert(true);
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    // glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        assert(false);
    }

    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "OpenGL Version: " << version << std::endl;

    if (!isExtensionSupported("GL_ARB_texture_compression_bptc")) {
        std::cerr << "ASTC compression extension is not enabled!" << std::endl;
    }
    else {
        std::cout << "ASTC compression extension is enabled!" << std::endl;
    }

    glfwSwapInterval(1);


    // During init, enable debug output
    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );
}

b32 glfw_window::handle_input()
{
    glfwPollEvents();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        input::set_key_state(key::ESCAPE, input_state::down);
        return true;
    } else
    {
        input::set_key_state(key::ESCAPE, input_state::up);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        input::set_key_state(key::w, input_state::down);
    } else
    {
        input::set_key_state(key::w, input_state::up);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        input::set_key_state(key::s, input_state::down);
    } else
    {
        input::set_key_state(key::s, input_state::up);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        input::set_key_state(key::a, input_state::down);
    } else
    {
        input::set_key_state(key::a, input_state::up);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        input::set_key_state(key::d, input_state::down);
    } else
    {
        input::set_key_state(key::d, input_state::up);
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        input::set_key_state(key::p, input_state::down);
    } else
    {
        input::set_key_state(key::p, input_state::up);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        input::set_key_state(key::r, input_state::down);
    } else
    {
        input::set_key_state(key::r, input_state::up);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        input::set_key_state(key::LCTRL, input_state::down);
    } else
    {
        input::set_key_state(key::LCTRL, input_state::up);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        input::set_key_state(key::LSHIFT, input_state::down);
    } else
    {
        input::set_key_state(key::LSHIFT, input_state::up);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        input::set_key_state(key::SPACE, input_state::down);
    } else
    {
        input::set_key_state(key::SPACE, input_state::up);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        input::set_key_state(key::UP, input_state::down);
    }
    else
    {
        input::set_key_state(key::UP, input_state::up);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        input::set_key_state(key::DOWN, input_state::down);
    }
    else
    {
        input::set_key_state(key::DOWN, input_state::up);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        input::set_key_state(key::LEFT, input_state::down);
    }
    else
    {
        input::set_key_state(key::LEFT, input_state::up);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        input::set_key_state(key::RIGHT, input_state::down);
    }
    else
    {
        input::set_key_state(key::RIGHT, input_state::up);
    }
    return false;
}

b32 glfw_window::is_gamepad_connected()
{
    return b32();
}

// void glfw_window::update()
// {
//     previous_time = glfwGetTime();
//     srand(previous_time);

//     b32 close_window = false;
//     while (!glfwWindowShouldClose(window))
//     {
//         glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//         glClear(GL_COLOR_BUFFER_BIT);

//         close_window = handle_input();

//         f64 time = (f64) glfwGetTime();
//         f32 dt = previous_time - time;
//         previous_time = time;
//     }
// }

b32 glfw_window::should_close()
{
    return glfwWindowShouldClose(window);
}

f64 glfw_window::get_time()
{
    return (f64)glfwGetTime();
}

void glfw_window::swap_buffers()
{
    glfwSwapBuffers(window);
}

void glfw_window::close()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}
