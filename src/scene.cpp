#include "scene.h"

namespace atl
{
    void scene::init(game_state* s)
    {
        state = s;

        start();
    }
}