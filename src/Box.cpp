#include "Box.h"

#include "game_state.h"

namespace atl
{
    void
    Box::init(game_state& state)
    {
        set_position({640, 400});

        texture_param ground_param{ texture_wrap::REPEAT, texture_filter::NEAREST_NEIGHBOR, texture_type::DIFFUSE, 0, texture_format::RGBA, texture_format::RGBA };

        sprite s;
        s.texture.create("../assets/textures/container2.png", ground_param);
        s.color = vec4(1.0f);
        s.size = vec2(25.0f, 25.0f);
        s.tiling_factor = 1.0f;
        set_sprite(new sprite(s));

        ref<physics_body2d> body = state.physics.create_body2d(this, get_position().x, get_position().y, get_transform().rotation.z, body_type::dynamic_body);
        add_physics_body(body);
        ref<collision_shape2d> shape = state.physics.add_shape2d(body, get_sprite()->size.x, get_sprite()->size.y);
        add_collision_shape(shape);

        box_drop = state.audio.create_sound("../assets/sound/box_drop.wav");
    }

    void
    Box::update(f32 dt, game_state& state)
    {
        if (play == true)
        {
            box_drop->start();
            play = false;
            played = true;
        }
    }

    void
    Box::on_collision_enter(collision_event& collision)
    {
        if (!played)
        {
            play = true;
            played = false;
        }
    }
}