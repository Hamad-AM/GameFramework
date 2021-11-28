#include "common.h"

#include "game.h"
#include "input.h"

#include <iostream>

game::game()
{
}

void game::initialize(input_system* input_system)
{
    input = input_system;
    // s = con.load_shader("sprite", "sprite.vs", "sprite.fs");
    // sprite_render.initialize(&s);
    
    // texture_param param{texture_wrap::CLAMP_TO_EDGE, texture_filter::NEAREST_NEIGHBOR, texture_type::DIFFUSE, 0, texture_format::RGB, texture_format::RGB};
    // texture = con.load_texture("test.png", param);
    camera_position = glm::vec3(1.0f, 1.0f, 0.0f);
    cam.update(camera_position);
    cam.set_projection(glm::ortho(-1.6f,  1.6f, -0.9f, 0.9f, -1.0f, 1.0f));

    sprite_render.initialize();
    texture_param param = {texture_wrap::CLAMP_TO_EDGE, texture_filter::BILINEAR, texture_type::DIFFUSE, 0, texture_format::RGBA, texture_format::RGBA};
    texture.create("test.png", param);
}

void game::update(f32 game_time)
{
    if (input->is_key_down(key::d))
        camera_position.x += 0.01f;
    else if (input->is_key_down(key::a))
        camera_position.x -= 0.01f;
    else if (input->is_key_down(key::w))
        camera_position.y += 0.01f;
    else if (input->is_key_down(key::s))
        camera_position.y -= 0.01f;
}

void game::draw(f32 game_time)
{
    cam.update(camera_position);
    std::cout << camera_position.x << " " << camera_position.y << " " << camera_position.z << std::endl;
    sprite_render.draw_sprite(cam, texture, glm::vec2(1.0f, -1.0f));
}

void game::close()
{
    sprite_render.render_delete();
}