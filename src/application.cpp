#include "common.h"

#include "application.h"
#include "input.h"
#include "components.h"

#include "box2d/box2d.h"

#include <iostream>
#include <map>

// ref<physics_object> create_bird(glm::vec3 start_position)
// {
//     ref<bird> b(new bird());
//     texture_param param = {texture_wrap::CLAMP_TO_EDGE, texture_filter::NEAREST_NEIGHBOR, texture_type::DIFFUSE, 0, texture_format::RGBA, texture_format::RGBA};
//     b->get_sprite().texture.create("bird.png", param);
//     s32 width = b->get_sprite().texture.width();
//     s32 height = b->get_sprite().texture.height();
//     b->get_sprite().size = glm::vec2(width * 5.0f, height * 5.0f);
//     b->get_transform().position = start_position;
//     b->get_transform().rotation = glm::vec3(0.0f, 0.0f, 0.0f);

//     return b;
// }

// ref<background> create_background(glm::vec3 start_position)
// {
//     ref<background> p(new background());
//     texture_param param = {texture_wrap::CLAMP_TO_BORDER, texture_filter::NEAREST_NEIGHBOR, texture_type::DIFFUSE, 0, texture_format::RGBA, texture_format::RGBA};
//     p->get_sprite().texture.create("background.png", param);
//     s32 width = p->get_sprite().texture.width();
//     s32 height = p->get_sprite().texture.height();
//     p->get_sprite().size = glm::vec2( pixel2screen * width * 1.0f, pixel2screen * height * 1.0f);
//     p->get_transform().position = start_position;
//     p->get_transform().rotation = glm::vec3(0.0f, 0.0f, 0.0f);

//     return p;
// }



application::application()
{
    
}

application::~application()
{
}

void application::initialize()
{
    // s = con.load_shader("sprite", "sprite.vs", "sprite.fs");
    // sprite_render.initialize(&s);
    
    // texture_param param{texture_wrap::CLAMP_TO_EDGE, texture_filter::NEAREST_NEIGHBOR, texture_type::DIFFUSE, 0, texture_format::RGB, texture_format::RGB};
    // texture = con.load_texture("test.png", param);
    camera_position = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.update(camera_position);
    // camera.set_projection(glm::ortho(-1.6f,  1.6f, -0.9f, 0.9f, -1.0f, 1.0f));
    camera.set_projection(glm::ortho(0.0f,  1280.0f, 0.0f, 720.0f, -1.0f, 1.0f));

    // ref<physics_object> b = create_bird(glm::vec3(1.0f, 0.6f, 0.0f));
    // physics_system.create_body2d(b, physics_body_component::body_type::dynamic_body)

    renderer.initialize();

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
    
    renderer.begin_sprite(camera);
    
    // for (ref<entity> ro : entities)
    // {
    //     if (ro->get_sprite())
    //     {
    //         glm::vec2 position(ro->get_transform().position.x, ro->get_transform().position.y);
    //         renderer.draw_sprite(ro->get_sprite()->texture, position, ro->get_sprite()->size, ro->get_transform().rotation.z);
    //     }
    // }
    renderer.end_sprite();


    renderer.begin_text(camera);

    renderer.draw_text("Hello World!", 640, 320, 1, {0.8, 0.05, 0.4});

    renderer.end_text();
}

void application::close()
{
    renderer.render_delete();
}
