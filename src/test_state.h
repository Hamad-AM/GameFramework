#pragma once

#include "common.h"

#include "game_state.h"

namespace atl
{
    class test_state : public game_state
    {
        void scene() override;
    };
}