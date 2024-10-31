#pragma once

#include <unordered_map>

#include "common.h"

#include "entity.h"

namespace atl
{
    class scene
    {
    public:
        void init(game_state* state);

        virtual void start() {}

        void update(f32 dt);

        void destroy();

    private:
        game_state* state;
    };
}