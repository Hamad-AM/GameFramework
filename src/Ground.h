#pragma once

#include "common.h"
#include "entity.h"

namespace atl
{
    class Ground : public entity
    {
    public:
        Ground() {}

        void init(renderer* render, audio_system* audio) override;

        void update(f32 dt) override;

    private:
    };
}