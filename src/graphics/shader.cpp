#include "shader.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

    Shader::Shader()
    {}

    void
    Shader::compile(const char* vs, const char* fs)
    {
        shader_name_ = vs;

        // Create an empty vertex shader handle
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

        std::string vs_src = read_file_to_string(vs);
        std::string fs_src = read_file_to_string(fs);

        // Send the vertex shader source code to GL
        // Note that std::string's .c_str is NULL character terminated.
        const GLchar* source = vs_src.c_str();
        glShaderSource(vertex_shader, 1, &source, 0);

        // Compile the vertex shader
        glCompileShader(vertex_shader);

        GLint is_compiled = 0;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &is_compiled);
        if (is_compiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, &infoLog[0]);

            // We don't need the shader anymore.
            glDeleteShader(vertex_shader);

            // Use the infoLog as you see fit.
            std::cout << "Vertex shader compilation failure!" << std::endl;
            std::cout << (const char*)infoLog.data() << std::endl;

            // In this simple program, we'll just leave
            return;
        }

        // Create an empty fragment shader handle
        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        // Send the fragment shader source code to GL
        // Note that std::string's .c_str is NULL character terminated.
        source = (const GLchar*)fs_src.c_str();
        glShaderSource(fragment_shader, 1, &source, 0);

        // Compile the fragment shader
        glCompileShader(fragment_shader);

        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_compiled);
        if (is_compiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(fragment_shader, maxLength, &maxLength, &infoLog[0]);

            // We don't need the shader anymore.
            glDeleteShader(fragment_shader);
            // Either of them. Don't leak shaders.
            glDeleteShader(vertex_shader);

            // Use the infoLog as you see fit.
            std::cout << "Fragment shader compilation failure!" << std::endl;
            std::cout << (const char*)infoLog.data() << std::endl;

            // In this simple program, we'll just leave
            return;
        }

        // Vertex and fragment shaders are successfully compiled.
        // Now time to link them together into a program.
        // Get a program object.
        GLuint program = glCreateProgram();

        program_ = program;

        // Attach our shaders to our program
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);

        // Link our program
        glLinkProgram(program);

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

            // We don't need the program anymore.
            glDeleteProgram(program);
            // Don't leak shaders either.
            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shader);

            // Use the infoLog as you see fit.
            std::cout << "Shader link failure!" << std::endl;
            std::cout << (const char*)infoLog.data() << std::endl;

            // In this simple program, we'll just leave
            return;
        }

        // Always detach shaders after a successful link.
        glDetachShader(program, vertex_shader);
        glDetachShader(program, fragment_shader);
        unbind();
    }

    void
    Shader::delete_shader()
    {
        glDeleteProgram(program_);
    }

    void
    Shader::bind()
    {
        glUseProgram(program_);
    }

    void
    Shader::unbind()
    {
        glUseProgram(0);
    }

    s32
    Shader::uniform_location(const char* name)
    {
        s32 location = glGetUniformLocation(program_, name);
        if (location == -1)
        {
            std::cout << "Uniform " << name << " not found for shader " << shader_name_ << std::endl;
            assert(false);
        }
        return location;
    }

    void
    Shader::uniform_float(const char* name, const f32 value)
    {
        glUniform1f(uniform_location(name), value);
    }

    void
    Shader::uniform_int(const char* name, const s32 value)
    {
        glUniform1i(uniform_location(name), value);
    }
    void
    Shader::uniform_vector2f(const char* name, const vec2& value)
    {
        glUniform2f(uniform_location(name), value.x, value.y);
    }

    void 
    Shader::uniform_vector3f   (const char* name, const vec3& value)
    {
        glUniform3f(uniform_location(name), value.x, value.y, value.z);
    }
    void
    Shader::uniform_vector4f   (const char* name, const vec4& value)
    {
        glUniform4f(uniform_location(name), value.x, value.y, value.z, value.w);
    }
    void
    Shader::uniform_matrix4    (const char* name, const mat4& value)
    {
        glUniformMatrix4fv(uniform_location(name), 1, GL_FALSE, glm::value_ptr(value));
    }


    std::string read_file_to_string(const char* path)
    {
        std::string s = "";
        std::ifstream s_stream(path, std::ios::in);
        if (!s_stream.is_open())
        {
            std::cout << "Could not read/load file " << path << ". " << std::endl;
            assert(true);
        }

        std::string line = "";
        while (!s_stream.eof())
        {
            std::getline(s_stream, line);
            s.append(line + "\n");
        }

        s_stream.close();
        return s;
    }
