#include "test_state.h"

namespace atl
{
    void test_state::scene()
    {
        add_entity(new Ground());
        add_entity(new Box());
    }
}