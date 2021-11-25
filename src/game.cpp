#include "common.h"

#include "game.h"
#include "content.h"

game::game()
{
}

void game::initialize()
{
    s = con.load_shader("sprite", "sprite.vs", "sprite.fs");
    sprite_render.initialize(&s);
    
    texture_param param{texture_wrap::CLAMP_TO_EDGE, texture_filter::NEAREST_NEIGHBOR, texture_type::DIFFUSE, 0, texture_format::RGB, texture_format::RGB};
    texture = con.load_texture("test.png", param);
    glm::vec3 position(0.0f, 0.0f, 1.0f);
    cam.update(position);
    cam.set_projection(glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, -1.0f, 1.0f));
}

void game::update(f32 game_time)
{

}

void game::draw(f32 game_time)
{
    cam.update(cam.position());
    sprite_render.draw_sprite(cam, texture, glm::vec2(1.0f, 1.0f));
}