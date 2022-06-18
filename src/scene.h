#pragma once

#include <unordered_map>

#include "common.h"

#include "entity.h"

namespace atl
{
    class scene
    {
    public:
        void init(physics_system* physics);

        void add_entity(const char* tag, entity* entity);
        void remove_entity(const char* tag);

        void add_camera2d(camera2d* camera);

        void draw(f32 dt);
        void update(f32 dt);
        void sim(f32 dt);

    private:
        std::unordered_map<const char*, entity> _entities;
        camera2d* _cam2d;
        physics_system* _physics_system;
    };
}