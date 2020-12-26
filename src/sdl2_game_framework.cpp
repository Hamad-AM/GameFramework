#include "sdl2_game_framework.h"

#include <Windows.h>
#include <assert.h>
#include <string>
#include <fstream>
#include <vector>

#include "input.h"

static uint32 screen_width = 1280;
static uint32 screen_height = 720; 

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

    glEnable(GL_DEPTH_TEST);
}

void bind_index_buffer(IndexBuffer* index_buffer)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->id);
}

void create_index_buffer(IndexBuffer* index_buffer, uint32* indices, uint32 count)
{
    glCreateBuffers(1, &index_buffer->id);
    bind_index_buffer(index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count, indices, GL_STATIC_DRAW);
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

uint32 shader_type_size(ShaderType type)
{
    switch(type)
    {
        case SHADER_FLOAT:
            return sizeof(float);
        default:
            return 0;
    }
}

GLenum convert_shadertype_to_gltype(ShaderType type)
{
    switch(type)
    {
        case ShaderType::SHADER_FLOAT: return GL_FLOAT;
        default: return 0;
    }
}

void assign_vertex_attrib_layout(BufferLayoutAttrib* layout, uint32 number_of_attrib)
{
    uint32 last_data_stride = 0;
    uint32 number_of_elements = 0;
    for (uint32 index = 0; index < number_of_attrib; ++index)
    {
        number_of_elements += layout[index].count;
    }

    for (uint32 index = 0; index < number_of_attrib; ++index)
    {
        glVertexAttribPointer(index,
                              layout[index].count,
                              convert_shadertype_to_gltype(layout[index].type),
                              GL_FALSE,
                              number_of_elements * shader_type_size(layout[index].type),
                              (void*)(last_data_stride));
        glEnableVertexAttribArray(index);
        last_data_stride += layout[index].count * shader_type_size(layout[index].type);
    }
}

void create_vertex_array(VertexArray* vertex_array)
{
    glGenVertexArrays(1, &vertex_array->id);
    bind_vertex_array(vertex_array);
}

void bind_shader(Shader* shader)
{
    glUseProgram(shader->program);
}

void unbind_shader(Shader* shader)
{
    glUseProgram(0);
}

void delete_shader(Shader* shader)
{
    glDeleteProgram(shader->program);
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
        printf(infoLog.data());

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
        printf(infoLog.data());

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
        printf(infoLog.data());

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

void upload_uniform_bool(const char* name, bool value, Shader* shader)
{
    uint32 location = glGetUniformLocation(shader->program, name);
    assert(location == -1);
    glUniform1i(location, (int32)value);
}

void upload_uniform_int32(const char* name, int32 value, Shader* shader)
{
    uint32 location = glGetUniformLocation(shader->program, name);
    assert(location == -1);
    glUniform1i(location, value);
}

void upload_uniform_float(const char* name, float value, Shader* shader)
{
    uint32 location = glGetUniformLocation(shader->program, name);
    //assert(location == -1);
    glUniform1f(location, value);
}

void upload_uniform_mat4(const char* name, glm::mat4 mat4, Shader* shader)
{
    uint32 location = glGetUniformLocation(shader->program, name);
    if (location == -1)
        printf("No such uniform known as %s", name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat4));
}

void upload_uniform_vec3(const char* name, glm::vec3 vec3, Shader* shader)
{
    uint32 location = glGetUniformLocation(shader->program, name);
    if (location == -1)
        printf("Uniform not found");
    glUniform3fv(location, 1, glm::value_ptr(vec3));
}
void upload_uniform_vec4(const char* name, glm::vec4 vec4, Shader* shader)
{
    uint32 location = glGetUniformLocation(shader->program, name);
    if (location == -1)
        printf("Uniform not found");
    glUniform3fv(location, 1, glm::value_ptr(vec4));
}



void clear_screen()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLenum convert_to_gl_filter(TextureFilter filter)
{
    switch(filter)
    {
        case TextureFilter::NEAREST_NEIGHBOR:
            return GL_NEAREST;
        case TextureFilter::BILINEAR:
            return GL_LINEAR;
        default:
            return GL_LINEAR;
    }
}
GLenum convert_to_gl_wrap(TextureWrap wrap) 
{
    switch(wrap)
    {
        case TextureWrap::CLAMP_TO_BORDER:
            return GL_CLAMP_TO_BORDER;
        case TextureWrap::CLAMP_TO_EDGE:
            return GL_CLAMP_TO_EDGE;
        case TextureWrap::MIRRORED_REPEAT:
            return GL_MIRRORED_REPEAT;
        case TextureWrap::REPEAT:
            return GL_REPEAT;
        default:
            return GL_CLAMP_TO_EDGE;
    }
}

LoadedTexture2D load_texture(const char* file_path, TextureFilter filter, TextureWrap wrapping)
{
    stbi_set_flip_vertically_on_load(1);

    LoadedTexture2D loaded_texture = {};
    loaded_texture.filter = filter;
    loaded_texture.wrap = wrapping;
    loaded_texture.image_data = stbi_load(file_path, &loaded_texture.width, &loaded_texture.height, &loaded_texture.number_of_channels, 0);

    assert(loaded_texture.image_data);

    return loaded_texture;
}

void free_loaded_texture(LoadedTexture2D* loaded_texture)
{
    stbi_image_free(loaded_texture->image_data);
    *loaded_texture = {};
}

void create_texture(Texture2D* texture, LoadedTexture2D* loaded_texture)
{
    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, convert_to_gl_filter(loaded_texture->filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, convert_to_gl_filter(loaded_texture->filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, convert_to_gl_wrap(loaded_texture->wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, convert_to_gl_wrap(loaded_texture->wrap));

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, loaded_texture->width, loaded_texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, loaded_texture->image_data);

    texture->loaded_texture = loaded_texture;
}

void bind_texture(Texture2D* texture, uint32 slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture->id);
}

void unbind_texture(Texture2D* texture)
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void delete_texture(Texture2D* texture)
{
    glDeleteTextures(1, &texture->id);
}
void handle_input(Input* game_input, SDL_Event* event, bool* running)
{

    while(SDL_PollEvent(event) != 0)
    {
        if (event->type == SDL_QUIT)
        {
            *running = true;
        }
        else if (event->type == SDL_KEYDOWN)
        {
            switch(event->key.keysym.sym)
            {
                case SDLK_a:
                    game_input->keyboard.keys[Keys::a] = ButtonState::Down;
                    break;
                case SDLK_b:
                    game_input->keyboard.keys[Keys::b] = ButtonState::Down;
                    break;
                case SDLK_c:
                    game_input->keyboard.keys[Keys::c] = ButtonState::Down;
                    break;
                case SDLK_d:
                    game_input->keyboard.keys[Keys::d] = ButtonState::Down;
                    break;
                case SDLK_e:
                    game_input->keyboard.keys[Keys::e] = ButtonState::Down;
                    break;
                case SDLK_f:
                    game_input->keyboard.keys[Keys::f] = ButtonState::Down;
                    break;
                case SDLK_g:
                    game_input->keyboard.keys[Keys::g] = ButtonState::Down;
                    break;
                case SDLK_h:
                    game_input->keyboard.keys[Keys::h] = ButtonState::Down;
                    break;
                case SDLK_i:
                    game_input->keyboard.keys[Keys::i] = ButtonState::Down;
                    break;
                case SDLK_j:
                    game_input->keyboard.keys[Keys::j] = ButtonState::Down;
                    break;
                case SDLK_k:
                    game_input->keyboard.keys[Keys::k] = ButtonState::Down;
                    break;
                case SDLK_l:
                    game_input->keyboard.keys[Keys::l] = ButtonState::Down;
                    break;
                case SDLK_m:
                    game_input->keyboard.keys[Keys::m] = ButtonState::Down;
                    break;
                case SDLK_n:
                    game_input->keyboard.keys[Keys::n] = ButtonState::Down;
                    break;
                case SDLK_o:
                    game_input->keyboard.keys[Keys::o] = ButtonState::Down;
                    break;
                case SDLK_p:
                    game_input->keyboard.keys[Keys::p] = ButtonState::Down;
                    break;
                case SDLK_q:
                    game_input->keyboard.keys[Keys::q] = ButtonState::Down;
                    break;
                case SDLK_r:
                    game_input->keyboard.keys[Keys::r] = ButtonState::Down;
                    break;
                case SDLK_s:
                    game_input->keyboard.keys[Keys::s] = ButtonState::Down;
                    break;
                case SDLK_t:
                    game_input->keyboard.keys[Keys::t] = ButtonState::Down;
                    break;
                case SDLK_u:
                    game_input->keyboard.keys[Keys::u] = ButtonState::Down;
                    break;
                case SDLK_v:
                    game_input->keyboard.keys[Keys::v] = ButtonState::Down;
                    break;
                case SDLK_w:
                    game_input->keyboard.keys[Keys::w] = ButtonState::Down;
                    break;
                case SDLK_x:
                    game_input->keyboard.keys[Keys::x] = ButtonState::Down;
                    break;
                case SDLK_y:
                    game_input->keyboard.keys[Keys::y] = ButtonState::Down;
                    break;
                case SDLK_z:
                    game_input->keyboard.keys[Keys::z] = ButtonState::Down;
                    break;
                case SDLK_BACKSPACE:
                    game_input->keyboard.keys[Keys::BACKSPACE] = ButtonState::Down;
                    break;
                case SDLK_TAB:
                    game_input->keyboard.keys[Keys::TAB] = ButtonState::Down;
                    break;
                case SDLK_SPACE:
                    game_input->keyboard.keys[Keys::SPACE] = ButtonState::Down;
                    break;
                case SDLK_RIGHT:
                    game_input->keyboard.keys[Keys::RIGHT] = ButtonState::Down;
                    break;
                case SDLK_LEFT:
                    game_input->keyboard.keys[Keys::LEFT] = ButtonState::Down;
                    break;
                case SDLK_DOWN:
                    game_input->keyboard.keys[Keys::DOWN] = ButtonState::Down;
                    break;
                case SDLK_UP:
                    game_input->keyboard.keys[Keys::UP] = ButtonState::Down;
                    break;
                case SDLK_LCTRL:
                    game_input->keyboard.keys[Keys::LCTRL] = ButtonState::Down;
                    break;
                case SDLK_LSHIFT:
                    game_input->keyboard.keys[Keys::LSHIFT] = ButtonState::Down;
                    break;
                case SDLK_LALT:
                    game_input->keyboard.keys[Keys::LALT] = ButtonState::Down;
                    break;
                default:
                    break;
            }
        }
        else if (event->type == SDL_KEYUP)
        {
            switch(event->key.keysym.sym)
            {
                case SDLK_a:
                    game_input->keyboard.keys[Keys::a] = ButtonState::Up;
                    break;
                case SDLK_b:
                    game_input->keyboard.keys[Keys::b] = ButtonState::Up;
                    break;
                case SDLK_c:
                    game_input->keyboard.keys[Keys::c] = ButtonState::Up;
                    break;
                case SDLK_d:
                    game_input->keyboard.keys[Keys::d] = ButtonState::Up;
                    break;
                case SDLK_e:
                    game_input->keyboard.keys[Keys::e] = ButtonState::Up;
                    break;
                case SDLK_f:
                    game_input->keyboard.keys[Keys::f] = ButtonState::Up;
                    break;
                case SDLK_g:
                    game_input->keyboard.keys[Keys::g] = ButtonState::Up;
                    break;
                case SDLK_h:
                    game_input->keyboard.keys[Keys::h] = ButtonState::Up;
                    break;
                case SDLK_i:
                    game_input->keyboard.keys[Keys::i] = ButtonState::Up;
                    break;
                case SDLK_j:
                    game_input->keyboard.keys[Keys::j] = ButtonState::Up;
                    break;
                case SDLK_k:
                    game_input->keyboard.keys[Keys::k] = ButtonState::Up;
                    break;
                case SDLK_l:
                    game_input->keyboard.keys[Keys::l] = ButtonState::Up;
                    break;
                case SDLK_m:
                    game_input->keyboard.keys[Keys::m] = ButtonState::Up;
                    break;
                case SDLK_n:
                    game_input->keyboard.keys[Keys::n] = ButtonState::Up;
                    break;
                case SDLK_o:
                    game_input->keyboard.keys[Keys::o] = ButtonState::Up;
                    break;
                case SDLK_p:
                    game_input->keyboard.keys[Keys::p] = ButtonState::Up;
                    break;
                case SDLK_q:
                    game_input->keyboard.keys[Keys::q] = ButtonState::Up;
                    break;
                case SDLK_r:
                    game_input->keyboard.keys[Keys::r] = ButtonState::Up;
                    break;
                case SDLK_s:
                    game_input->keyboard.keys[Keys::s] = ButtonState::Up;
                    break;
                case SDLK_t:
                    game_input->keyboard.keys[Keys::t] = ButtonState::Up;
                    break;
                case SDLK_u:
                    game_input->keyboard.keys[Keys::u] = ButtonState::Up;
                    break;
                case SDLK_v:
                    game_input->keyboard.keys[Keys::v] = ButtonState::Up;
                    break;
                case SDLK_w:
                    game_input->keyboard.keys[Keys::w] = ButtonState::Up;
                    break;
                case SDLK_x:
                    game_input->keyboard.keys[Keys::x] = ButtonState::Up;
                    break;
                case SDLK_y:
                    game_input->keyboard.keys[Keys::y] = ButtonState::Up;
                    break;
                case SDLK_z:
                    game_input->keyboard.keys[Keys::z] = ButtonState::Up;
                    break;
                case SDLK_BACKSPACE:
                    game_input->keyboard.keys[Keys::BACKSPACE] = ButtonState::Up;
                    break;
                case SDLK_TAB:
                    game_input->keyboard.keys[Keys::TAB] = ButtonState::Up;
                    break;
                case SDLK_SPACE:
                    game_input->keyboard.keys[Keys::SPACE] = ButtonState::Up;
                    break;
                case SDLK_RIGHT:
                    game_input->keyboard.keys[Keys::RIGHT] = ButtonState::Up;
                    break;
                case SDLK_LEFT:
                    game_input->keyboard.keys[Keys::LEFT] = ButtonState::Up;
                    break;
                case SDLK_DOWN:
                    game_input->keyboard.keys[Keys::DOWN] = ButtonState::Up;
                    break;
                case SDLK_UP:
                    game_input->keyboard.keys[Keys::UP] = ButtonState::Up;
                    break;
                case SDLK_LCTRL:
                    game_input->keyboard.keys[Keys::LCTRL] = ButtonState::Up;
                    break;
                case SDLK_LSHIFT:
                    game_input->keyboard.keys[Keys::LSHIFT] = ButtonState::Up;
                    break;
                case SDLK_LALT:
                    game_input->keyboard.keys[Keys::LALT] = ButtonState::Up;
                    break;
                default:
                    break;
            }
        }
        else if (event->type == SDL_MOUSEMOTION)
        {
            int32 x, y;
            SDL_GetMouseState(&x, &y);
            game_input->mouse.x = x;
            game_input->mouse.y = y;
        }
        else if (event->type == SDL_MOUSEBUTTONDOWN)
        {
           if (event->button.button == SDL_BUTTON_LEFT)
           {
               game_input->mouse.left_button = ButtonState::Down;
           } 
           else if (event->button.button == SDL_BUTTON_RIGHT)
           {
               game_input->mouse.right_button = ButtonState::Down;
           }
           else if (event->button.button == SDL_BUTTON_MIDDLE)
           {
               game_input->mouse.middle_button = ButtonState::Down;
           }
        }
        else if (event->type == SDL_MOUSEBUTTONUP)
        {
           if (event->button.button == SDL_BUTTON_LEFT)
           {
               game_input->mouse.left_button = ButtonState::Up;
           } 
           else if (event->button.button == SDL_BUTTON_RIGHT)
           {
               game_input->mouse.right_button = ButtonState::Up;
           }
           else if (event->button.button == SDL_BUTTON_MIDDLE)
           {
               game_input->mouse.middle_button = ButtonState::Up;
           }
        }
        else if (event->type == SDL_MOUSEWHEEL)
        {
            game_input->mouse.scroll_wheel_value = y;
        }
    }
    // TODO: GamePad input
}

Input setup_game_input()
{
    Input game_input = {};
    if (SDL_NumJoysticks() < 1)
    {
        game_input.game_pad_connected = false;
    }
    else
    {
        game_input.game_pad_connected = true;
    }

    for (int i = 0; i < 104; ++i)
    {
        game_input.keyboard.keys[i] = {};
        game_input.keyboard.keys[i] = ButtonState::Up;
    }

    game_input.mouse = {};
    game_input.mouse.left_button = ButtonState::Up;
    game_input.mouse.middle_button = ButtonState::Up;
    game_input.mouse.right_button = ButtonState::Up;
    game_input.mouse.scroll_wheel_value = 0.0f;
    game_input.mouse.x = 0.0f;
    game_input.mouse.y = 0.0f;

    for (int i = 0; i < 4; ++i)
    {
        game_input.game_pad[i] = {};
        game_input.game_pad[i].A = ButtonState::Up;
        game_input.game_pad[i].B = ButtonState::Up;
        game_input.game_pad[i].back = ButtonState::Up;
        game_input.game_pad[i].left_shoulder = ButtonState::Up;
        game_input.game_pad[i].right_shoulder = ButtonState::Up;
        game_input.game_pad[i].start = ButtonState::Up;
        game_input.game_pad[i].X = ButtonState::Up;
        game_input.game_pad[i].Y = ButtonState::Up;
        game_input.game_pad[i].down = ButtonState::Up;
        game_input.game_pad[i].left = ButtonState::Up;
        game_input.game_pad[i].right = ButtonState::Up;
        game_input.game_pad[i].up = ButtonState::Up;
        game_input.game_pad[i].left_stick = ButtonState::Up;
        game_input.game_pad[i].right_stick = ButtonState::Up;

        game_input.game_pad[i].left_stick_x = 0.0f;
        game_input.game_pad[i].left_stick_y = 0.0f;
        game_input.game_pad[i].right_stick_x = 0.0f;
        game_input.game_pad[i].right_stick_y = 0.0f;
        game_input.game_pad[i].left_trigger = 0.0f;
        game_input.game_pad[i].right_trigger = 0.0f;

    }
    return game_input;
}

void init_texture_cube(TextureCube* cube)
{
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,1.0f, 0.0f,

        0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };

    memcpy(cube->vertices, vertices, sizeof(vertices));

    uint32 indices[6] = {
        0, 1, 3,
        1, 2, 3 
    };

    BufferLayoutAttrib layout[3] = {
        {ShaderType::SHADER_FLOAT, 3, false},
        {ShaderType::SHADER_FLOAT, 3, false},
        {ShaderType::SHADER_FLOAT, 2, false},
    };

    create_vertex_array(&cube->va);
    create_vertex_buffer(&cube->vb, vertices, sizeof(vertices));

    assign_vertex_attrib_layout(layout, 3);   
    load_shader(&cube->shader, "..\\data\\shaders\\Texture.glsl");

    unbind_vertex_array(&cube->va);
    unbind_vertex_buffer();
    unbind_index_buffer();

}
void init_cube(Cube* cube)
{
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,          0.5f, -0.5f, -0.5f,          0.5f,  0.5f, -0.5f,          0.5f,  0.5f, -0.5f,          -0.5f,  0.5f, -0.5f,          -0.5f, -0.5f, -0.5f,          -0.5f, -0.5f,  0.5f,          0.5f, -0.5f,  0.5f,          0.5f,  0.5f,  0.5f,          0.5f,  0.5f,  0.5f,          -0.5f,  0.5f,  0.5f,          -0.5f, -0.5f,  0.5f,          -0.5f,  0.5f,  0.5f,          -0.5f,  0.5f, -0.5f,          -0.5f, -0.5f, -0.5f,          -0.5f, -0.5f, -0.5f,          -0.5f, -0.5f,  0.5f,          -0.5f,  0.5f,  0.5f,          0.5f,  0.5f,  0.5f,          0.5f,  0.5f, -0.5f,          0.5f, -0.5f, -0.5f,          0.5f, -0.5f, -0.5f,          0.5f, -0.5f,  0.5f,          0.5f,  0.5f,  0.5f,          -0.5f, -0.5f, -0.5f,          0.5f, -0.5f, -0.5f,          0.5f, -0.5f,  0.5f,          0.5f, -0.5f,  0.5f,          -0.5f, -0.5f,  0.5f,          -0.5f, -0.5f, -0.5f,          -0.5f,  0.5f, -0.5f,          0.5f,  0.5f, -0.5f,          0.5f,  0.5f,  0.5f,          0.5f,  0.5f,  0.5f,          -0.5f,  0.5f,  0.5f,          -0.5f,  0.5f, -0.5f,      };

    memcpy(cube->vertices, vertices, sizeof(vertices));

    uint32 indices[6] = {
        0, 1, 3,
        1, 2, 3 
    };

    BufferLayoutAttrib layout[1] = {
        {ShaderType::SHADER_FLOAT, 3, false},
    };

    create_vertex_array(&cube->va);
    create_vertex_buffer(&cube->vb, vertices, sizeof(vertices));

    assign_vertex_attrib_layout(layout, 1);

    load_shader(&cube->shader, "..\\data\\shaders\\TestLightSource.glsl");

    unbind_vertex_array(&cube->va);
    unbind_vertex_buffer();
    unbind_index_buffer();
}
void update_camera(PerspectiveCamera* camera)
{
    camera->view = glm::translate(glm::mat4(1.0f), camera->position);
    camera->projection_view = camera->projection * camera->view;
}

void draw_cube(Cube* cube, PerspectiveCamera* camera)
{
    bind_vertex_array(&cube->va);
    bind_shader(&cube->shader);
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, cube->position);
    model = glm::scale(model, glm::vec3(0.5f));


    upload_uniform_mat4("u_ProjectionViewMat", camera->projection_view, &cube->shader);
    upload_uniform_mat4("u_ModelMat", model, &cube->shader);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    unbind_shader(&cube->shader);
    unbind_vertex_array(&cube->va);
}

void draw_texture_cube(TextureCube* cube, PerspectiveCamera* camera, LightMaterial light)
{
    bind_texture(&cube->texture, 0);
    bind_vertex_array(&cube->va);
    bind_shader(&cube->shader);

    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, cube->position);


    upload_uniform_mat4("u_ProjectionViewMat", camera->projection_view, &cube->shader);
    upload_uniform_mat4("u_ModelMat", model, &cube->shader);
    upload_uniform_vec3("material.diffuse", cube->material.diffuse , &cube->shader);
    upload_uniform_vec3("material.ambient", cube->material.ambient , &cube->shader);
    upload_uniform_vec3("material.specular", cube->material.specular , &cube->shader);
    upload_uniform_float("material.shininess", cube->material.shininess , &cube->shader);
    upload_uniform_vec3("light.diffuse", light.diffuse, &cube->shader);
    upload_uniform_vec3("light.ambient", light.ambient, &cube->shader);
    upload_uniform_vec3("light.specular",  light.specular, &cube->shader);
    upload_uniform_vec3("light.position", light.position, &cube->shader);
    upload_uniform_vec3("light.color", light.position, &cube->shader);
    
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    unbind_shader(&cube->shader);
    unbind_vertex_array(&cube->va);
    unbind_texture(&cube->texture);
}

int main()
{
    WindowHandle win_handle = {screen_width, screen_height}; 
    int32 init_out = SDL_Init(SDL_INIT_VIDEO);
    assert(!(init_out < 0));
    
    win_handle.window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, win_handle.width, win_handle.height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    assert(win_handle.window != NULL);
    
    initGL(&win_handle);

    TextureCube text_cube;
    Cube light_source;

    LoadedTexture2D loaded_texture = load_texture("..\\data\\textures\\cobble_stone.png", TextureFilter::BILINEAR, TextureWrap::CLAMP_TO_EDGE);

    init_cube(&light_source);
    init_texture_cube(&text_cube);
    text_cube.texture = {};
    create_texture(&text_cube.texture, &loaded_texture);

    text_cube.position = glm::vec3(-1.0f, -0.5f, -5.0f);
    light_source.position = glm::vec3(-1.0f, -0.1f, 3.0f);
    
    LightMaterial light = {v3(0.0f, 0.0f, 0.0f), v3(1.0f, 1.0f, 1.0f), v3(0.2f, 0.2f, 0.2f), v3(0.5f, 0.5f, 0.5f), v3(1.0f, 1.0f, 1.0f)};

    text_cube.material = {v3(1.0f, 0.5f, 0.31f), v3(1.0f, 0.5f, 0.31f), v3(0.5f, 0.5f, 0.5f), 32.0f};

    //upload_uniform_mat4("transform", trans, &shader);

    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float),(void*)0);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float),(void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);

    PerspectiveCamera camera = {};

    camera.fov = 45.0f;
    camera.position = glm::vec3(0.0f, 0.0f, 3.0f);

    float yaw = -90.0f;
    float pitch = 0.0f;

    float mouse_last_x = 640;
    float mouse_last_y = 360;

    camera.projection = glm::perspective(glm::radians(camera.fov), (float)1280/(float)720, 0.1f, 100.0f);

    SDL_Event event;
    Input game_input = setup_game_input();

    bool running = false;

    while (!running)
    {
        handle_input(&game_input, &event, &running);
        clear_screen();

        if (game_input.keyboard.keys[Keys::a] == ButtonState::Down)
            camera.position += glm::vec3(0.05f, 0.0f, 0.0f);
        if (game_input.keyboard.keys[Keys::d] == ButtonState::Down)
            camera.position += glm::vec3(-0.05f, 0.0f, 0.0f);
        if (game_input.keyboard.keys[Keys::w] == ButtonState::Down)
            camera.position += glm::vec3(0.0f, 0.0f, 0.05f);
        if (game_input.keyboard.keys[Keys::s] == ButtonState::Down)
            camera.position += glm::vec3(0.0f, 0.0f, -0.05f);
        if (game_input.keyboard.keys[Keys::SPACE] == ButtonState::Down)
            camera.position += glm::vec3(0.0f, -0.05f, 0.0f);
        if (game_input.keyboard.keys[Keys::LSHIFT] == ButtonState::Down)
            camera.position += glm::vec3(0.0f, 0.05f, 0.0f);

        update_camera(&camera);

        draw_cube(&light_source, &camera);
        light.position = light_source.position;
        draw_texture_cube(&text_cube, &camera, light);

        SDL_GL_SwapWindow(win_handle.window);
    }
    
    delete_vertex_array(&text_cube.va);
    delete_vertex_array(&light_source.va);
    delete_vertex_buffer(&text_cube.vb);
    delete_vertex_buffer(&light_source.vb);
    delete_texture(&text_cube.texture);
    delete_shader(&text_cube.shader);
    delete_shader(&light_source.shader);

    free_loaded_texture(&loaded_texture);

    SDL_DestroyWindow(win_handle.window);
    win_handle.window = nullptr;
    SDL_GL_DeleteContext(win_handle.context);
    win_handle.context = NULL;

    SDL_Quit();

    return 0;
}