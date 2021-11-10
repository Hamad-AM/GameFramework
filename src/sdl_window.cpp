// #include <assert.h>

// #include <stb_image.h>
// #include <glad/glad.h>

// #include "math.h"
// #include "input.h"

// #include "sdl_window.h"

// void
// SDLWindow::InitWindow(u32 width, u32 height, const char* title)
// {
//     s32 init_out = SDL_Init(SDL_INIT_VIDEO);
//     assert(!(init_out < 0));

//     window_ = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
//     assert(window_ != NULL);

//     height_ = height;
//     width_ = width;

//     SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
//     SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
//     SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

//     context_ = SDL_GL_CreateContext(window_);
//     assert(context_ != NULL);

//     gladLoadGLLoader(SDL_GL_GetProcAddress);

//     // Use VSync 
//     // TODO: Make this optional to user
//     SDL_GL_SetSwapInterval(1);

//     glEnable(GL_DEPTH_TEST);

//     game.Init();
// }

// b32
// SDLWindow::HandleInput()
// {
//     while(SDL_PollEvent(&event_) != 0)
//     {
//         if (event_.type == SDL_QUIT)
//         {
//             return false;
//         }
//         else if (event_.type == SDL_KEYDOWN)
//         {
//             switch(event_.key.keysym.sym)
//             {
//                 case SDLK_a:
//                     input.keyboard().keys[Keys::a] = ButtonState::Down;
//                     break;
//                 case SDLK_b:
//                     input.keyboard().keys[Keys::b] = ButtonState::Down;
//                     break;
//                 case SDLK_c:
//                     input.keyboard().keys[Keys::c] = ButtonState::Down;
//                     break;
//                 case SDLK_d:
//                     input.keyboard().keys[Keys::d] = ButtonState::Down;
//                     break;
//                 case SDLK_e:
//                     input.keyboard().keys[Keys::e] = ButtonState::Down;
//                     break;
//                 case SDLK_f:
//                     input.keyboard().keys[Keys::f] = ButtonState::Down;
//                     break;
//                 case SDLK_g:
//                     input.keyboard().keys[Keys::g] = ButtonState::Down;
//                     break;
//                 case SDLK_h:
//                     input.keyboard().keys[Keys::h] = ButtonState::Down;
//                     break;
//                 case SDLK_i:
//                     input.keyboard().keys[Keys::i] = ButtonState::Down;
//                     break;
//                 case SDLK_j:
//                     input.keyboard().keys[Keys::j] = ButtonState::Down;
//                     break;
//                 case SDLK_k:
//                     input.keyboard().keys[Keys::k] = ButtonState::Down;
//                     break;
//                 case SDLK_l:
//                     input.keyboard().keys[Keys::l] = ButtonState::Down;
//                     break;
//                 case SDLK_m:
//                     input.keyboard().keys[Keys::m] = ButtonState::Down;
//                     break;
//                 case SDLK_n:
//                     input.keyboard().keys[Keys::n] = ButtonState::Down;
//                     break;
//                 case SDLK_o:
//                     input.keyboard().keys[Keys::o] = ButtonState::Down;
//                     break;
//                 case SDLK_p:
//                     input.keyboard().keys[Keys::p] = ButtonState::Down;
//                     break;
//                 case SDLK_q:
//                     input.keyboard().keys[Keys::q] = ButtonState::Down;
//                     break;
//                 case SDLK_r:
//                     input.keyboard().keys[Keys::r] = ButtonState::Down;
//                     break;
//                 case SDLK_s:
//                     input.keyboard().keys[Keys::s] = ButtonState::Down;
//                     break;
//                 case SDLK_t:
//                     input.keyboard().keys[Keys::t] = ButtonState::Down;
//                     break;
//                 case SDLK_u:
//                     input.keyboard().keys[Keys::u] = ButtonState::Down;
//                     break;
//                 case SDLK_v:
//                     input.keyboard().keys[Keys::v] = ButtonState::Down;
//                     break;
//                 case SDLK_w:
//                     input.keyboard().keys[Keys::w] = ButtonState::Down;
//                     break;
//                 case SDLK_x:
//                     input.keyboard().keys[Keys::x] = ButtonState::Down;
//                     break;
//                 case SDLK_y:
//                     input.keyboard().keys[Keys::y] = ButtonState::Down;
//                     break;
//                 case SDLK_z:
//                     input.keyboard().keys[Keys::z] = ButtonState::Down;
//                     break;
//                 case SDLK_BACKSPACE:
//                     input.keyboard().keys[Keys::BACKSPACE] = ButtonState::Down;
//                     break;
//                 case SDLK_TAB:
//                     input.keyboard().keys[Keys::TAB] = ButtonState::Down;
//                     break;
//                 case SDLK_SPACE:
//                     input.keyboard().keys[Keys::SPACE] = ButtonState::Down;
//                     break;
//                 case SDLK_RIGHT:
//                     input.keyboard().keys[Keys::RIGHT] = ButtonState::Down;
//                     break;
//                 case SDLK_LEFT:
//                     input.keyboard().keys[Keys::LEFT] = ButtonState::Down;
//                     break;
//                 case SDLK_DOWN:
//                     input.keyboard().keys[Keys::DOWN] = ButtonState::Down;
//                     break;
//                 case SDLK_UP:
//                     input.keyboard().keys[Keys::UP] = ButtonState::Down;
//                     break;
//                 case SDLK_LCTRL:
//                     input.keyboard().keys[Keys::LCTRL] = ButtonState::Down;
//                     break;
//                 case SDLK_LSHIFT:
//                     input.keyboard().keys[Keys::LSHIFT] = ButtonState::Down;
//                     break;
//                 case SDLK_LALT:
//                     input.keyboard().keys[Keys::LALT] = ButtonState::Down;
//                     break;
//                 case SDLK_ESCAPE:
//                     input.keyboard().keys[Keys::ESCAPE] = ButtonState::Down;
//                     break;
//                 default:
//                     break;
//             }
//         }
//         else if (event_.type == SDL_KEYUP)
//         {
//             switch(event_.key.keysym.sym)
//             {
//                 case SDLK_a:
//                     input.keyboard().keys[Keys::a] = ButtonState::Up;
//                     break;
//                 case SDLK_b:
//                     input.keyboard().keys[Keys::b] = ButtonState::Up;
//                     break;
//                 case SDLK_c:
//                     input.keyboard().keys[Keys::c] = ButtonState::Up;
//                     break;
//                 case SDLK_d:
//                     input.keyboard().keys[Keys::d] = ButtonState::Up;
//                     break;
//                 case SDLK_e:
//                     input.keyboard().keys[Keys::e] = ButtonState::Up;
//                     break;
//                 case SDLK_f:
//                     input.keyboard().keys[Keys::f] = ButtonState::Up;
//                     break;
//                 case SDLK_g:
//                     input.keyboard().keys[Keys::g] = ButtonState::Up;
//                     break;
//                 case SDLK_h:
//                     input.keyboard().keys[Keys::h] = ButtonState::Up;
//                     break;
//                 case SDLK_i:
//                     input.keyboard().keys[Keys::i] = ButtonState::Up;
//                     break;
//                 case SDLK_j:
//                     input.keyboard().keys[Keys::j] = ButtonState::Up;
//                     break;
//                 case SDLK_k:
//                     input.keyboard().keys[Keys::k] = ButtonState::Up;
//                     break;
//                 case SDLK_l:
//                     input.keyboard().keys[Keys::l] = ButtonState::Up;
//                     break;
//                 case SDLK_m:
//                     input.keyboard().keys[Keys::m] = ButtonState::Up;
//                     break;
//                 case SDLK_n:
//                     input.keyboard().keys[Keys::n] = ButtonState::Up;
//                     break;
//                 case SDLK_o:
//                     input.keyboard().keys[Keys::o] = ButtonState::Up;
//                     break;
//                 case SDLK_p:
//                     input.keyboard().keys[Keys::p] = ButtonState::Up;
//                     break;
//                 case SDLK_q:
//                     input.keyboard().keys[Keys::q] = ButtonState::Up;
//                     break;
//                 case SDLK_r:
//                     input.keyboard().keys[Keys::r] = ButtonState::Up;
//                     break;
//                 case SDLK_s:
//                     input.keyboard().keys[Keys::s] = ButtonState::Up;
//                     break;
//                 case SDLK_t:
//                     input.keyboard().keys[Keys::t] = ButtonState::Up;
//                     break;
//                 case SDLK_u:
//                     input.keyboard().keys[Keys::u] = ButtonState::Up;
//                     break;
//                 case SDLK_v:
//                     input.keyboard().keys[Keys::v] = ButtonState::Up;
//                     break;
//                 case SDLK_w:
//                     input.keyboard().keys[Keys::w] = ButtonState::Up;
//                     break;
//                 case SDLK_x:
//                     input.keyboard().keys[Keys::x] = ButtonState::Up;
//                     break;
//                 case SDLK_y:
//                     input.keyboard().keys[Keys::y] = ButtonState::Up;
//                     break;
//                 case SDLK_z:
//                     input.keyboard().keys[Keys::z] = ButtonState::Up;
//                     break;
//                 case SDLK_BACKSPACE:
//                     input.keyboard().keys[Keys::BACKSPACE] = ButtonState::Up;
//                     break;
//                 case SDLK_TAB:
//                     input.keyboard().keys[Keys::TAB] = ButtonState::Up;
//                     break;
//                 case SDLK_SPACE:
//                     input.keyboard().keys[Keys::SPACE] = ButtonState::Up;
//                     break;
//                 case SDLK_RIGHT:
//                     input.keyboard().keys[Keys::RIGHT] = ButtonState::Up;
//                     break;
//                 case SDLK_LEFT:
//                     input.keyboard().keys[Keys::LEFT] = ButtonState::Up;
//                     break;
//                 case SDLK_DOWN:
//                     input.keyboard().keys[Keys::DOWN] = ButtonState::Up;
//                     break;
//                 case SDLK_UP:
//                     input.keyboard().keys[Keys::UP] = ButtonState::Up;
//                     break;
//                 case SDLK_LCTRL:
//                     input.keyboard().keys[Keys::LCTRL] = ButtonState::Up;
//                     break;
//                 case SDLK_LSHIFT:
//                     input.keyboard().keys[Keys::LSHIFT] = ButtonState::Up;
//                     break;
//                 case SDLK_LALT:
//                     input.keyboard().keys[Keys::LALT] = ButtonState::Up;
//                     break;
//                 case SDLK_ESCAPE:
//                     input.keyboard().keys[Keys::ESCAPE] = ButtonState::Up;
//                     break;
//                 default:
//                     break;
//             }
//         }
//         else if (event_.type == SDL_MOUSEMOTION)
//         {
//             s32 x, y;
//             SDL_GetMouseState(&x, &y);
//             input.mouse().x = x;
//             input.mouse().y = y;
//         }
//         else if (event_.type == SDL_MOUSEBUTTONDOWN)
//         {
//            if (event_.button.button == SDL_BUTTON_LEFT)
//            {
//                input.mouse().left_button = ButtonState::Down;
//            } 
//            else if (event_.button.button == SDL_BUTTON_RIGHT)
//            {
//                input.mouse().right_button = ButtonState::Down;
//            }
//            else if (event_.button.button == SDL_BUTTON_MIDDLE)
//            {
//                input.mouse().middle_button = ButtonState::Down;
//            }
//         }
//         else if (event_.type == SDL_MOUSEBUTTONUP)
//         {
//            if (event_.button.button == SDL_BUTTON_LEFT)
//            {
//                input.mouse().left_button = ButtonState::Up;
//            } 
//            else if (event_.button.button == SDL_BUTTON_RIGHT)
//            {
//                input.mouse().right_button = ButtonState::Up;
//            }
//            else if (event_.button.button == SDL_BUTTON_MIDDLE)
//            {
//                input.mouse().middle_button = ButtonState::Up;
//            }
//         }
//         else if (event_.type == SDL_MOUSEWHEEL)
//         {
//             input.mouse().scroll_wheel_value = y;
//         }
//     }
//     // TODO: GamePad input

//     return true;
// }

// b32
// SDLWindow::IsGamepadConnected() 
// {
//     return !(SDL_NumJoysticks() < 1);
// }

// void
// SDLWindow::UpdateWindow() 
// {
//     game.Update();
//     game.Draw();
//     SDL_GL_SwapWindow(window_);
// }

// void
// SDLWindow::CloseWindow() 
// {
//     SDL_DestroyWindow(window_);
//     window_ = NULL;
//     SDL_Quit();
// }

// int main()
// {
//     SDLWindow window;

//     window.InitWindow(1280, 720, "Bob's good game");

//     b32 running = true;
//     while (!running)
//     {
//         running = window.HandleInput();
//         window.UpdateWindow();
//     }
//     window.CloseWindow();
// }