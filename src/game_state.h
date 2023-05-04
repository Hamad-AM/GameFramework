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
        game_state(application* app) : app(m_app) {}

        std::vector<ref<entity>> entities;

        physics_system physics;
        mini_audio_system audio;
        
        renderer render;
        camera2d camera;
        vec3 camera_position;

        void init();
        void destroy();

        template<typename T>
        void switch_state() { m_app->switch_state<T>(); }

    private:
        virtual void scene() {}
        void init_systems();
        ref<entity> add_entity(entity* e);
        void initialize_entities();

        const application* m_app;
    };


}