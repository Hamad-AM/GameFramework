#include "Box.h"

namespace atl
{
    void
    Box::init(renderer* render, audio_system* audio)
    {
        set_position({640, 400});

        texture_param ground_param{ texture_wrap::REPEAT, texture_filter::NEAREST_NEIGHBOR, TextureType::ALBEDO, 0, texture_format::RGBA, texture_format::RGBA };

        sprite s;
        s.texture.create("../assets/textures/container2.png", ground_param);
        s.color = vec4(1.0f);
        s.size = vec2(25.0f, 25.0f);
        s.tiling_factor = 1.0f;
        set_sprite(new sprite(s));

        box_drop = audio->create_sound("../assets/sound/box_drop.wav");
    }

    void
    Box::update(f32 dt)
    {
        if (play == true)
        {
            box_drop->start();
            play = false;
            played = true;
        }
    }
}