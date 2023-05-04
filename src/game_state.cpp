#include "game_state.h"

namespace atl
{
    void game_state::destroy()
    {
        entities.clear();
        physics.destroy();
        audio.destroy();
        camera_position = vec3(1.0f);
        camera.update(state->camera_position);
    }
    void game_state::init()
    {
        init_systems();
        scene();
        initialize_entities();
    }

    void game_state::init_systems()
    {
        state.camera_position = vec3(0.0f, 0.0f, 0.0f);
        state.camera.set_projection(glm::ortho(0.0f,  1280.0f, 0.0f, 720.0f, -1.0f, 1.0f));
        
        state.render.initialize();
        state.physics.initialize2d();

        state.audio.init();
    }

    ref>entity> game_state::add_entity(entity* e)
    {
        ref<entity> ref_entity(e);
        state.entities.push_back(ref_entity);
    }

    void game_state::initialize_entities()
    {
        for (u32 i = 0; i < state.entities.size(); ++i)
        {
            state.entities[i]->init(state);
        }
    }
}