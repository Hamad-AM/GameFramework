#include "shader.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

shader::shader()
{}

void
shader::compile(const char* vs, const char* fs)
{
    shader_name_ = vs;

    // Create an empty vertex shader handle
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);


    std::ifstream fs_file;
    fs_file.open(fs);
    std::stringstream fs_ss;
    fs_ss << fs_file.rdbuf();
    fs_file.close();
    std::string fs_src = fs_ss.str();

    std::ifstream vs_file;
    std::string vs_src;    
    std::stringstream vs_ss;
    vs_file.open(vs);
    vs_ss << vs_file.rdbuf();
    vs_file.close();
    vs_src = vs_ss.str();

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
        printf("Vertex shader compilation failure!");
        printf(infoLog.data());

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
        printf("Fragment shader compilation failure!");
        printf(infoLog.data());

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
        printf("Shader link failure!");
        printf(infoLog.data());

        // In this simple program, we'll just leave
        return;
    }

    // Always detach shaders after a successful link.
    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);
}

shader::~shader()
{
    glDeleteProgram(program_);
}

void
shader::bind()
{
    glUseProgram(program_);
}

void
shader::unbind()
{
    glUseProgram(0);
}

s32
shader::uniform_location(const char* name)
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
shader::uniform_float(const char* name, f32 value)
{
    glUniform1f(uniform_location(name), value);
}

void
shader::uniform_int(const char* name, s32 value)
{
    glUniform1i(uniform_location(name), value);
}
void
shader::uniform_vector2f(const char* name, glm::vec2& value)
{
    glUniform2f(uniform_location(name), value.x, value.y);
}

void 
shader::uniform_vector3f   (const char* name, glm::vec3& value)
{
    glUniform3f(uniform_location(name), value.x, value.y, value.z);
}
void
shader::uniform_vector4f   (const char* name, glm::vec4& value)
{
    glUniform4f(uniform_location(name), value.x, value.y, value.z, value.w);
}
void
shader::uniform_matrix4    (const char* name, glm::mat4& value)
{
    glUniformMatrix4fv(uniform_location(name), 1, GL_FALSE, glm::value_ptr(value));
}