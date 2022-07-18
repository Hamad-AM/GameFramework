#include "b2_misc.h"

namespace box2d
{
        //
    void gl_box2d::sCheckGLError()
    {
        GLenum errCode = glGetError();
        if (errCode != GL_NO_ERROR)
        {
            fprintf(stderr, "OpenGL error = %d\n", errCode);
            assert(false);
        }
    }

    // Prints shader compilation errors
    void gl_box2d::sPrintLog(GLuint object)
    {
        GLint log_length = 0;
        if (glIsShader(object))
            glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
        else if (glIsProgram(object))
            glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
        else
        {
            fprintf(stderr, "printlog: Not a shader or a program\n");
            return;
        }

        char* log = (char*)malloc(log_length);

        if (glIsShader(object))
            glGetShaderInfoLog(object, log_length, NULL, log);
        else if (glIsProgram(object))
            glGetProgramInfoLog(object, log_length, NULL, log);

        fprintf(stderr, "%s", log);
        free(log);
    }


    //
    GLuint gl_box2d::sCreateShaderFromString(const char* source, GLenum type)
    {
        GLuint res = glCreateShader(type);
        const char* sources[] = { source };
        glShaderSource(res, 1, sources, NULL);
        glCompileShader(res);
        GLint compile_ok = GL_FALSE;
        glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
        if (compile_ok == GL_FALSE)
        {
            fprintf(stderr, "Error compiling shader of type %d!\n", type);
            sPrintLog(res);
            glDeleteShader(res);
            return 0;
        }

        return res;
    }

    // 
    GLuint gl_box2d::sCreateShaderProgram(const char* vs, const char* fs)
    {
        GLuint vsId = sCreateShaderFromString(vs, GL_VERTEX_SHADER);
        GLuint fsId = sCreateShaderFromString(fs, GL_FRAGMENT_SHADER);
        assert(vsId != 0 && fsId != 0);

        GLuint programId = glCreateProgram();
        glAttachShader(programId, vsId);
        glAttachShader(programId, fsId);
        glBindFragDataLocation(programId, 0, "color");
        glLinkProgram(programId);

        glDeleteShader(vsId);
        glDeleteShader(fsId);

        GLint status = GL_FALSE;
        glGetProgramiv(programId, GL_LINK_STATUS, &status);
        assert(status != GL_FALSE);

        return programId;
    }
}