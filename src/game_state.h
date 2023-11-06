#pragma once

#include "common.h"
#include "camera.h"
#include "physics/physics_system.h"
#include "graphics/renderer2d.h"
#include "audio/mini_audio.h"

#include <array>

namespace atl
{
    class entity;

    struct game_state
    {
        game_state() {}

        std::vector<ref<entity>> entities;

        physics_system physics;
        mini_audio_system audio;
        
        renderer render;
        camera2d camera;
        vec3 camera_position;

        void init();
        void destroy();
        ref<entity> add_entity(entity* e);

    private:
        virtual void scene() {}
        void init_systems();
        void initialize_entities();

    };


}