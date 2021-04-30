#include <memoryapi.h>
#include <assert.h>

#include <stb_image.h>
#include <glad/glad.h>

#include "math.h"
#include "input.h"

#include "sdl_window.h"

void
SDLWindow::InitWindow(u32 width, u32 height, const char* title)
{
    s32 init_out = SDL_Init(SDL_INIT_VIDEO);
    assert(!(init_out < 0));

    window_ = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    assert(window_ != NULL);

    height_ = height;
    width_ = width;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    context_ = SDL_GL_CreateContext(window_);
    assert(context_ != NULL);

    gladLoadGLLoader(SDL_GL_GetProcAddress);

    // Use VSync 
    // TODO: Make this optional to user
    SDL_GL_SetSwapInterval(1);

    glEnable(GL_DEPTH_TEST);

}

b32
SDLWindow::HandleInput(Input* game_input)
{
    while(SDL_PollEvent(&event_) != 0)
    {
        if (event_.type == SDL_QUIT)
        {
            return false;
        }
        else if (event_.type == SDL_KEYDOWN)
        {
            switch(event_.key.keysym.sym)
            {
                case SDLK_a:
                    game_input->keyboard().keys[Keys::a] = ButtonState::Down;
                    break;
                case SDLK_b:
                    game_input->keyboard().keys[Keys::b] = ButtonState::Down;
                    break;
                case SDLK_c:
                    game_input->keyboard().keys[Keys::c] = ButtonState::Down;
                    break;
                case SDLK_d:
                    game_input->keyboard().keys[Keys::d] = ButtonState::Down;
                    break;
                case SDLK_e:
                    game_input->keyboard().keys[Keys::e] = ButtonState::Down;
                    break;
                case SDLK_f:
                    game_input->keyboard().keys[Keys::f] = ButtonState::Down;
                    break;
                case SDLK_g:
                    game_input->keyboard().keys[Keys::g] = ButtonState::Down;
                    break;
                case SDLK_h:
                    game_input->keyboard().keys[Keys::h] = ButtonState::Down;
                    break;
                case SDLK_i:
                    game_input->keyboard().keys[Keys::i] = ButtonState::Down;
                    break;
                case SDLK_j:
                    game_input->keyboard().keys[Keys::j] = ButtonState::Down;
                    break;
                case SDLK_k:
                    game_input->keyboard().keys[Keys::k] = ButtonState::Down;
                    break;
                case SDLK_l:
                    game_input->keyboard().keys[Keys::l] = ButtonState::Down;
                    break;
                case SDLK_m:
                    game_input->keyboard().keys[Keys::m] = ButtonState::Down;
                    break;
                case SDLK_n:
                    game_input->keyboard().keys[Keys::n] = ButtonState::Down;
                    break;
                case SDLK_o:
                    game_input->keyboard().keys[Keys::o] = ButtonState::Down;
                    break;
                case SDLK_p:
                    game_input->keyboard().keys[Keys::p] = ButtonState::Down;
                    break;
                case SDLK_q:
                    game_input->keyboard().keys[Keys::q] = ButtonState::Down;
                    break;
                case SDLK_r:
                    game_input->keyboard().keys[Keys::r] = ButtonState::Down;
                    break;
                case SDLK_s:
                    game_input->keyboard().keys[Keys::s] = ButtonState::Down;
                    break;
                case SDLK_t:
                    game_input->keyboard().keys[Keys::t] = ButtonState::Down;
                    break;
                case SDLK_u:
                    game_input->keyboard().keys[Keys::u] = ButtonState::Down;
                    break;
                case SDLK_v:
                    game_input->keyboard().keys[Keys::v] = ButtonState::Down;
                    break;
                case SDLK_w:
                    game_input->keyboard().keys[Keys::w] = ButtonState::Down;
                    break;
                case SDLK_x:
                    game_input->keyboard().keys[Keys::x] = ButtonState::Down;
                    break;
                case SDLK_y:
                    game_input->keyboard().keys[Keys::y] = ButtonState::Down;
                    break;
                case SDLK_z:
                    game_input->keyboard().keys[Keys::z] = ButtonState::Down;
                    break;
                case SDLK_BACKSPACE:
                    game_input->keyboard().keys[Keys::BACKSPACE] = ButtonState::Down;
                    break;
                case SDLK_TAB:
                    game_input->keyboard().keys[Keys::TAB] = ButtonState::Down;
                    break;
                case SDLK_SPACE:
                    game_input->keyboard().keys[Keys::SPACE] = ButtonState::Down;
                    break;
                case SDLK_RIGHT:
                    game_input->keyboard().keys[Keys::RIGHT] = ButtonState::Down;
                    break;
                case SDLK_LEFT:
                    game_input->keyboard().keys[Keys::LEFT] = ButtonState::Down;
                    break;
                case SDLK_DOWN:
                    game_input->keyboard().keys[Keys::DOWN] = ButtonState::Down;
                    break;
                case SDLK_UP:
                    game_input->keyboard().keys[Keys::UP] = ButtonState::Down;
                    break;
                case SDLK_LCTRL:
                    game_input->keyboard().keys[Keys::LCTRL] = ButtonState::Down;
                    break;
                case SDLK_LSHIFT:
                    game_input->keyboard().keys[Keys::LSHIFT] = ButtonState::Down;
                    break;
                case SDLK_LALT:
                    game_input->keyboard().keys[Keys::LALT] = ButtonState::Down;
                    break;
                case SDLK_ESCAPE:
                    game_input->keyboard().keys[Keys::ESCAPE] = ButtonState::Down;
                    break;
                default:
                    break;
            }
        }
        else if (event_.type == SDL_KEYUP)
        {
            switch(event_.key.keysym.sym)
            {
                case SDLK_a:
                    game_input->keyboard().keys[Keys::a] = ButtonState::Up;
                    break;
                case SDLK_b:
                    game_input->keyboard().keys[Keys::b] = ButtonState::Up;
                    break;
                case SDLK_c:
                    game_input->keyboard().keys[Keys::c] = ButtonState::Up;
                    break;
                case SDLK_d:
                    game_input->keyboard().keys[Keys::d] = ButtonState::Up;
                    break;
                case SDLK_e:
                    game_input->keyboard().keys[Keys::e] = ButtonState::Up;
                    break;
                case SDLK_f:
                    game_input->keyboard().keys[Keys::f] = ButtonState::Up;
                    break;
                case SDLK_g:
                    game_input->keyboard().keys[Keys::g] = ButtonState::Up;
                    break;
                case SDLK_h:
                    game_input->keyboard().keys[Keys::h] = ButtonState::Up;
                    break;
                case SDLK_i:
                    game_input->keyboard().keys[Keys::i] = ButtonState::Up;
                    break;
                case SDLK_j:
                    game_input->keyboard().keys[Keys::j] = ButtonState::Up;
                    break;
                case SDLK_k:
                    game_input->keyboard().keys[Keys::k] = ButtonState::Up;
                    break;
                case SDLK_l:
                    game_input->keyboard().keys[Keys::l] = ButtonState::Up;
                    break;
                case SDLK_m:
                    game_input->keyboard().keys[Keys::m] = ButtonState::Up;
                    break;
                case SDLK_n:
                    game_input->keyboard().keys[Keys::n] = ButtonState::Up;
                    break;
                case SDLK_o:
                    game_input->keyboard().keys[Keys::o] = ButtonState::Up;
                    break;
                case SDLK_p:
                    game_input->keyboard().keys[Keys::p] = ButtonState::Up;
                    break;
                case SDLK_q:
                    game_input->keyboard().keys[Keys::q] = ButtonState::Up;
                    break;
                case SDLK_r:
                    game_input->keyboard().keys[Keys::r] = ButtonState::Up;
                    break;
                case SDLK_s:
                    game_input->keyboard().keys[Keys::s] = ButtonState::Up;
                    break;
                case SDLK_t:
                    game_input->keyboard().keys[Keys::t] = ButtonState::Up;
                    break;
                case SDLK_u:
                    game_input->keyboard().keys[Keys::u] = ButtonState::Up;
                    break;
                case SDLK_v:
                    game_input->keyboard().keys[Keys::v] = ButtonState::Up;
                    break;
                case SDLK_w:
                    game_input->keyboard().keys[Keys::w] = ButtonState::Up;
                    break;
                case SDLK_x:
                    game_input->keyboard().keys[Keys::x] = ButtonState::Up;
                    break;
                case SDLK_y:
                    game_input->keyboard().keys[Keys::y] = ButtonState::Up;
                    break;
                case SDLK_z:
                    game_input->keyboard().keys[Keys::z] = ButtonState::Up;
                    break;
                case SDLK_BACKSPACE:
                    game_input->keyboard().keys[Keys::BACKSPACE] = ButtonState::Up;
                    break;
                case SDLK_TAB:
                    game_input->keyboard().keys[Keys::TAB] = ButtonState::Up;
                    break;
                case SDLK_SPACE:
                    game_input->keyboard().keys[Keys::SPACE] = ButtonState::Up;
                    break;
                case SDLK_RIGHT:
                    game_input->keyboard().keys[Keys::RIGHT] = ButtonState::Up;
                    break;
                case SDLK_LEFT:
                    game_input->keyboard().keys[Keys::LEFT] = ButtonState::Up;
                    break;
                case SDLK_DOWN:
                    game_input->keyboard().keys[Keys::DOWN] = ButtonState::Up;
                    break;
                case SDLK_UP:
                    game_input->keyboard().keys[Keys::UP] = ButtonState::Up;
                    break;
                case SDLK_LCTRL:
                    game_input->keyboard().keys[Keys::LCTRL] = ButtonState::Up;
                    break;
                case SDLK_LSHIFT:
                    game_input->keyboard().keys[Keys::LSHIFT] = ButtonState::Up;
                    break;
                case SDLK_LALT:
                    game_input->keyboard().keys[Keys::LALT] = ButtonState::Up;
                    break;
                case SDLK_ESCAPE:
                    game_input->keyboard().keys[Keys::ESCAPE] = ButtonState::Up;
                    break;
                default:
                    break;
            }
        }
        else if (event_.type == SDL_MOUSEMOTION)
        {
            s32 x, y;
            SDL_GetMouseState(&x, &y);
            game_input->mouse().x = x;
            game_input->mouse().y = y;
        }
        else if (event_.type == SDL_MOUSEBUTTONDOWN)
        {
           if (event_.button.button == SDL_BUTTON_LEFT)
           {
               game_input->mouse().left_button = ButtonState::Down;
           } 
           else if (event_.button.button == SDL_BUTTON_RIGHT)
           {
               game_input->mouse().right_button = ButtonState::Down;
           }
           else if (event_.button.button == SDL_BUTTON_MIDDLE)
           {
               game_input->mouse().middle_button = ButtonState::Down;
           }
        }
        else if (event_.type == SDL_MOUSEBUTTONUP)
        {
           if (event_.button.button == SDL_BUTTON_LEFT)
           {
               game_input->mouse().left_button = ButtonState::Up;
           } 
           else if (event_.button.button == SDL_BUTTON_RIGHT)
           {
               game_input->mouse().right_button = ButtonState::Up;
           }
           else if (event_.button.button == SDL_BUTTON_MIDDLE)
           {
               game_input->mouse().middle_button = ButtonState::Up;
           }
        }
        else if (event_.type == SDL_MOUSEWHEEL)
        {
            game_input->mouse().scroll_wheel_value = y;
        }
    }
    // TODO: GamePad input

    return true;
}

b32
SDLWindow::IsGamepadConnected() 
{
    return !(SDL_NumJoysticks() < 1);
}

void
SDLWindow::UpdateWindow() 
{
    SDL_GL_SwapWindow(window_);
}

void
SDLWindow::CloseWindow() 
{
    SDL_DestroyWindow(window_);
    window_ = NULL;
    SDL_Quit();
}

// Texture2D platform_load_texture(const char* file_path, TextureFilter filter, TextureWrap wrapping)
// {
//     stbi_set_flip_vertically_on_load(1);

//     Texture2D loaded_texture = {};
//     loaded_texture.filter = filter;
//     loaded_texture.wrap = wrapping;
//     loaded_texture.image_data = stbi_load(file_path, &loaded_texture.width, &loaded_texture.height, &loaded_texture.number_of_channels, 0);
//     loaded_texture.path = file_path;

//     assert(loaded_texture.image_data);
//     return loaded_texture;
// }

// void free_texture(Texture2D* texture)
// {
//     stbi_image_free(texture->image_data);
//     delete_texture(texture);
// }
