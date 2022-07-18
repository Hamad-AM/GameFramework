#pragma once

#include <cstdio>
#include <cassert>
#include <cstddef>
#include <cstdlib>

#include <glad/glad.h>

#define BUFFER_OFFSET(x)  ((const void*) (x))

namespace box2d
{
    class gl_box2d
    {
    public:
        static void sCheckGLError();

        // Prints shader compilation errors
        static void sPrintLog(GLuint object);

        //
        static GLuint sCreateShaderFromString(const char* source, GLenum type);

        // 
        static GLuint sCreateShaderProgram(const char* vs, const char* fs);

    };
}
