#include "sdl2_ahm.h"

#include <assert.h>

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

void setupGL()
{
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    VertexBuffer vertex_buffer;
    create_vertex_buffer(&vertex_buffer, vertices, sizeof(vertices));

    uint32 indices[] = { 0, 1, 0 };

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
    setupGL();

    SDL_Event event;
    bool running = false;

    while (!running)
    {
        while(SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
                running = true;
        }

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SDL_GL_SwapWindow(win_handle.window);
    }
    
    SDL_DestroyWindow(win_handle.window);
    win_handle.window = nullptr;
    SDL_GL_DeleteContext(win_handle.context);
    win_handle.context = NULL;

    SDL_Quit();

    return 0;
}