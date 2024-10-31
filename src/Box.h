#pragma once

#include "common.h"
#include "entity.h"
#include "audio.h"

namespace atl
{
    class Box : public entity
    {
    public:
        Box() {}

        void init(renderer* render, audio_system* audio) override;

        void update(f32 dt) override;


    private:
        ref<sound> box_drop;
        ref<sound_channel> box_drop_channel;
        b32 played = false;
        b32 play = false;
    };
}