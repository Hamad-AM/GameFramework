#pragma once

#include "common.h"
#include "entity.h"

namespace atl
{
    class Box : public entity
    {
    public:
        Box() {}

        void init(game_state& state) override;

        void update(f32 dt, game_state& state) override;

    private:
    };
}