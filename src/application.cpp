#include "common.h"

#include "application.h"
#include "input.h"
#include "components.h"

#include "box2d/box2d.h"

#include <iostream>
#include <map>

namespace alg
{
    application::application() {}

    application::~application() {}

    void application::initialize()
    {
        // s = con.load_shader("sprite", "sprite.vs", "sprite.fs");
        // sprite_render.initialize(&s);
        
        // texture_param param{texture_wrap::CLAMP_TO_EDGE, texture_filter::NEAREST_NEIGHBOR, texture_type::DIFFUSE, 0, texture_format::RGB, texture_format::RGB};
        // texture = con.load_texture("test.png", param);
        camera_position = vec3(0.0f, 0.0f, 0.0f);
        camera.update(camera_position);
        // camera.set_projection(glm::ortho(-1.6f,  1.6f, -0.9f, 0.9f, -1.0f, 1.0f));
        camera.set_projection(glm::ortho(0.0f,  1280.0f, 0.0f, 720.0f, -1.0f, 1.0f));

        // ref<physics_object> b = create_bird(glm::vec3(1.0f, 0.6f, 0.0f));
        // physics_system.create_body2d(b, physics_body_component::body_type::dynamic_body)

        render.initialize();
    }

    void application::update(f32 dt)
    {
        if (input::is_key_down(key::d))
            camera_position.x += 0.01f;
        else if (input::is_key_down(key::a))
            camera_position.x -= 0.01f;
        else if (input::is_key_down(key::w))
            camera_position.y += 0.01f;
        else if (input::is_key_down(key::s))
            camera_position.y -= 0.01f;
        else if (input::is_key_down(key::p))
            return;

        if (input::is_left_mouse_down())
        {
            mouse_event press_event{event_type::left_mouse_button_press, input::mouse_position()};
            event_system::get().post_all(event_type::left_mouse_button_press, &press_event);
        }
        mouse_event move_event{event_type::mouse_move, input::mouse_position()};
        event_system::get().post_all(event_type::mouse_move, &move_event);
    }

    void application::draw(f32 dt)
    {
        camera.update(camera_position);
        
        render.begin_sprite(camera);
        
        render.end_sprite();

        render.begin_text(camera);

        vec3 text_position = vec3(0.9, 0.9, 0.9);
        render.draw_text("Hello World!", 640, 320, 1, font_type::Montserrat, text_position);

        render.end_text();
    }

    void application::close()
    {
        render.render_delete();
    }
}