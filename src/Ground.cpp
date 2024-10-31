#include "Ground.h"

namespace atl
{
    void
    Ground::init(renderer* render, audio_system* audio)
    {
        set_position({640, 50});

        texture_param ground_param{ texture_wrap::REPEAT, texture_filter::NEAREST_NEIGHBOR, texture_type::DIFFUSE, 0, texture_format::RGBA, texture_format::RGBA };

        sprite s;
        s.texture.create("../assets/textures/container2.png", ground_param);
        s.color = vec4(1.0f);
        s.size = vec2(640.0f, 50.0f);
        s.tiling_factor = 1.0f;
        set_sprite(new sprite(s));

    }

    void
    Ground::update(f32 dt)
    {
        return;
    }
}