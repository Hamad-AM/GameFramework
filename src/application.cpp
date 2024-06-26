#include "common.h"

#include "application.h"
#include "input.h"
#include "components.h"
#include "start_up_state.h"

#include <iostream>
#include <map>
#include <chrono>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

namespace atl
{

    application::application() : state(new start_up_state(this)) {}

    application::~application()
    {
        delete window;
    }

    void application::initialize(u32 screen_width, u32 screen_height)
    {

        window = new glfw_window();
        screen_width_ = screen_width;
        screen_height_ = screen_height;
        window->initialize(screen_width_, screen_height_, "Bob McGee's Very Good Game");

        state->init();
        dbg.start();
    }

    void application::run()
    {
        previous_time = window->get_time();

        while (!window->should_close())
        {
            window->handle_input();
            // state.audio.update();

            f64 current_time = window->get_time();
            f64 dt = current_time - previous_time;
            previous_time = current_time;

            if (dt < 0)
                dt = 0.0001;
            
            update(dt);

            window->swap_buffers();
        }
    }

    void application::update(f32 dt)
    {
        if (input::is_key_down(key::d))
            state->camera_position.x += 0.01f;
        else if (input::is_key_down(key::a))
            state->camera_position.x -= 0.01f;
        else if (input::is_key_down(key::w))
            state->camera_position.y += 0.01f;
        else if (input::is_key_down(key::s))
            state->camera_position.y -= 0.01f;
        else if (input::is_key_down(key::p))
            return;

        if (input::is_left_mouse_down())
        {
            mouse_event press_event{event_type::left_mouse_button_press, input::mouse_position()};
            event_system::get()->post_all(event_type::left_mouse_button_press, &press_event);
        }
        mouse_event move_event{event_type::mouse_move, input::mouse_position()};
        event_system::get()->post_all(event_type::mouse_move, &move_event);

        state->camera.update(state->camera_position);

        state->physics.update(dt);

        for (u32 i = 0; i < state->entities.size(); ++i)
        {
            state->entities[i]->update(dt, state);
        }

        state->render.begin2d(state->camera);

        vec4 color{ 0.9, 0.9, 0.9, 1.0f };
        state->render.draw_text(std::to_string((u32)round(dt*1000)) + " ms", screen_width_*0.9, screen_height_*0.95, 0.5, font_type::Montserrat, color, &state->camera);
        // std::string memory_str = std::to_string(allocated_memory/1000) + " MB";
        // state.render.draw_text(memory_str, screen_width_*0.875, screen_height_*0.9, 0.5, font_type::Montserrat, color, &state.camera);

        for (u32 i = 0; i < state->entities.size(); ++i)
        {
            ref<entity> e = state->entities[i];
            if (e->get_sprite() != nullptr)
            {
                vec2 p(e->get_transform().position.x, e->get_transform().position.y);
                state->render.draw_texture(e->get_sprite()->texture, p, e->get_sprite()->size, e->get_transform().rotation.z, e->get_sprite()->color, e->get_sprite()->tiling_factor, &state->camera);
            }
        }

        dbg.draw_collisions(state);
        dbg.flush();
    
        state->render.end2d();
    }

    void application::switch_state()
    {
        state->destroy();
        state = make_ref<T>(this);
        state->init();
    }

    void application::close()
    {
        dbg.end();
        state->physics.destroy();

        state->render.render_delete();
        window->close();
    }
}