#include "sdl2_game_framework.h"

#include <assert.h>
#include <string>
#include <fstream>
#include <vector>

#include <glad/glad.h>

void initGL(WindowHandle* handle)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    handle->context = SDL_GL_CreateContext(handle->window);
    assert(handle->context != NULL);

    gladLoadGLLoader(SDL_GL_GetProcAddress);

    // Use VSync 
    // TODO: Make this optional to user
    SDL_GL_SetSwapInterval(1);
}

void bind_index_buffer(IndexBuffer* index_buffer)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->id);
}

void create_index_buffer(IndexBuffer* index_buffer, uint32* indices, uint32 count)
{
    glCreateBuffers(1, &index_buffer->id);
    bind_index_buffer(index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32), indices, GL_STATIC_DRAW);
}

void unbind_index_buffer()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void delete_index_buffer(IndexBuffer* index_buffer)
{
    glDeleteBuffers(1, &index_buffer->id);
}

void bind_vertex_buffer(VertexBuffer* vertex_buffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->id);
}

void create_vertex_buffer(VertexBuffer* vertex_buffer, float* vertices, uint32 size)
{
    glCreateBuffers(1, &vertex_buffer->id);
    bind_vertex_buffer(vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void unbind_vertex_buffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void delete_vertex_buffer(VertexBuffer* vertex_buffer)
{
    glDeleteBuffers(1, &vertex_buffer->id);
}

void bind_vertex_array(VertexArray* vertex_array)
{
    glBindVertexArray(vertex_array->id);
}

void delete_vertex_array(VertexArray* vertex_array)
{
    glDeleteVertexArrays(1, &vertex_array->id);
}

void unbind_vertex_array(VertexArray* vertex_array)
{
    glBindVertexArray(0);
}

void create_vertex_array(VertexArray* vertex_array)
{
    glGenVertexArrays(1, &vertex_array->id);
    bind_vertex_array(vertex_array);
}

void compile_shader(Shader* shader, const char* fragment_src, const char* vertex_src)
{
    // Create an empty vertex shader handle
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    // Send the vertex shader source code to GL
    // Note that std::string's .c_str is NULL character terminated.
    const GLchar* source = vertex_src;
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
        printf("{0}", infoLog.data());

        // In this simple program, we'll just leave
        return;
    }

    // Create an empty fragment shader handle
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    // Send the fragment shader source code to GL
    // Note that std::string's .c_str is NULL character terminated.
    source = (const GLchar*)fragment_src;
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
        printf("{0}", infoLog.data());

        // In this simple program, we'll just leave
        return;
    }

    // Vertex and fragment shaders are successfully compiled.
    // Now time to link them together into a program.
    // Get a program object.
    GLuint program = glCreateProgram();

    shader->program = program;

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
        printf("{0}", infoLog.data());

        // In this simple program, we'll just leave
        return;
    }

    // Always detach shaders after a successful link.
    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);
}

void load_shader(Shader* shader, const char* file_path)
{
    std::ifstream stream(file_path);
    std::string line;

    std::string fragment_shader = "";
    std::string vertex_shader = "";
    bool is_fragment = false;
    
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                is_fragment = false;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                is_fragment = true;
            }
        }
        else
        {
            if (is_fragment)
                fragment_shader += line + "\n";
            else
                vertex_shader += line + "\n";
        }
    }

    compile_shader(shader, fragment_shader.c_str(), vertex_shader.c_str());
}

void ClearScreen()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void setupGL(Shader* shader, VertexArray* vertex_array, VertexBuffer* vertex_buffer, IndexBuffer* index_buffer)
{
    float vertices[] = {
        0.5f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f,
       -0.5f,  0.5f, 0.0f,
   };

    create_vertex_array(vertex_array);
    create_vertex_buffer(vertex_buffer, vertices, sizeof(vertices));

    uint32 indices[] = {
        0, 1, 3,
        1, 2, 3 
    };

    create_index_buffer(index_buffer, indices, sizeof(indices));

    load_shader(shader, "data/shaders/Base.glsl");

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

int main()
{
    WindowHandle win_handle = {1280, 720}; 
    int32 init_out = SDL_Init(SDL_INIT_VIDEO);
    assert(!(init_out < 0));
    
    win_handle.window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, win_handle.width, win_handle.height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    assert(win_handle.window != NULL);
    
    initGL(&win_handle);

    VertexBuffer vertex_buffer;
    IndexBuffer index_buffer;
    VertexArray vertex_array;
    Shader shader;

    setupGL(&shader, &vertex_array, &vertex_buffer, &index_buffer);

    SDL_Event event;
    bool running = false;

    while (!running)
    {
        while(SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
                running = true;
        }

        ClearScreen();

        bind_vertex_array(&vertex_array); 
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        unbind_vertex_array(&vertex_array);

        SDL_GL_SwapWindow(win_handle.window);
    }
    
    delete_vertex_array(&vertex_array);
    delete_vertex_buffer(&vertex_buffer);
    delete_index_buffer(&index_buffer);

    SDL_DestroyWindow(win_handle.window);
    win_handle.window = nullptr;
    SDL_GL_DeleteContext(win_handle.context);
    win_handle.context = NULL;

    SDL_Quit();

    return 0;
}