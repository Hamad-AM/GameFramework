#include "test_state.h"

#include "Ground.h"
#include "Box.h"

namespace atl
{
    void test_state::scene()
    {
        ref<entity> ground = add_entity(new Ground());
        ref<entity> box = add_entity(new Box());
    }
}