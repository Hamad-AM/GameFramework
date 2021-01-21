#include "sdl2_anthology.h"

#include <windows.h>
#include <memoryapi.h>
#include <assert.h>

#include <string>
#include <fstream>
#include <vector>

#include "anthology.h"

#include "anthology_types.h"
#include "anthology_renderer.h"
#include "anthology.cpp"

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

void handle_input(Input* game_input, SDL_Event* event, bool* running)
{
    while(SDL_PollEvent(event) != 0)
    {
        if (event->type == SDL_QUIT)
        {
            *running = false;
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
                case SDLK_ESCAPE:
                    game_input->keyboard.keys[Keys::ESCAPE] = ButtonState::Down;
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
                case SDLK_ESCAPE:
                    game_input->keyboard.keys[Keys::ESCAPE] = ButtonState::Up;
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
    game_input.mouse.scroll_wheel_value = 0;
    game_input.mouse.x = 0;
    game_input.mouse.y = 0;

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



int main()
{
    WindowHandle win_handle = {screen_width, screen_height}; 
    int32 init_out = SDL_Init(SDL_INIT_VIDEO);
    assert(!(init_out < 0));
    
    win_handle.window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, win_handle.width, win_handle.height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    assert(win_handle.window != NULL);

    Memory game_memory = {};
    game_memory.permanent_storage_size = 64 * 1024 * 1024;
    game_memory.permanent_storage = VirtualAlloc(0, game_memory.permanent_storage_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    
    initGL(&win_handle);

    TextureCube text_cube;
    Cube light_source;

    LoadedTexture2D loaded_texture = load_texture("..\\..\\data\\textures\\container2.png", TextureFilter::BILINEAR, TextureWrap::CLAMP_TO_EDGE);
    LoadedTexture2D loaded_specular_map = load_texture("..\\..\\data\\textures\\container2_specular.png", TextureFilter::BILINEAR, TextureWrap::CLAMP_TO_EDGE);

    init_texture_cube(&text_cube);
    text_cube.material.diffuse = {};
    text_cube.material.specular = {};
    text_cube.material.shininess = 32.0f;
    create_texture(&text_cube.material.diffuse, &loaded_texture);
    text_cube.material.diffuse.slot = 0;
    create_texture(&text_cube.material.specular, &loaded_specular_map);
    text_cube.material.specular.slot = 1;

    text_cube.position = glm::vec3(-1.0f, -0.5f, -5.0f);
    
    Environment environment;
    environment.dir_light.direction = v3(0.0f, 0.0f, -0.2f);
    environment.dir_light.ambient = v3(0.2f, 0.2f, 0.2f);
    environment.dir_light.diffuse = v3(0.5f, 0.5f, 0.5f);
    environment.dir_light.specular = v3(1.0f, 1.0f, 1.0f);

    environment.spot_light.direction = v3(0.0f, 0.0f, -0.2f);
    environment.spot_light.position = v3(0.0f, 0.0f, 5.0f);
    environment.spot_light.ambient = v3(0.2f, 0.2f, 0.2f);
    environment.spot_light.specular = v3(1.0f, 1.0f, 1.0f);
    environment.spot_light.innerCutOff = glm::cos(glm::radians(25.0f));
    environment.spot_light.outerCutOff = glm::cos(glm::radians(35.0f));

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

    bool running = true;

    while (running)
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

        if (game_input.keyboard.keys[Keys::ESCAPE] == ButtonState::Down)
            running = false;

        game_update_and_render(&game_memory, &game_input);

        update_camera(&camera);

        draw_texture_cube(&text_cube, &camera, &environment);

        SDL_GL_SwapWindow(win_handle.window);
    }
    
    delete_vertex_array(&text_cube.va);
    delete_vertex_array(&light_source.va);
    delete_vertex_buffer(&text_cube.vb);
    delete_vertex_buffer(&light_source.vb);
    delete_texture(&text_cube.material.diffuse);
    delete_texture(&text_cube.material.specular);
    delete_shader(&text_cube.shader);
    delete_shader(&light_source.shader);

    free_loaded_texture(&loaded_texture);
    free_loaded_texture(&loaded_specular_map);

    SDL_DestroyWindow(win_handle.window);
    win_handle.window = nullptr;
    SDL_GL_DeleteContext(win_handle.context);
    win_handle.context = NULL;

    SDL_Quit();

    return 0;
}