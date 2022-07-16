#pragma once

#include <map>

#include <box2d/box2d.h>

#include "../common.h"
#include "../components.h"
#include "../entity.h"

#include "physics_body2d.h"
#include "collision_shape2d.h"
#include "collision_event.h"

namespace atl
{

    class physics_system
    {
    public:
        physics_system()
        {}

        ~physics_system() {}

        void initialize2d();

        physics_body2d* create_body2d(entity* entity, f32 x, f32 y, f32 rotation, body_type type);
        collision_shape2d* add_shape2d(physics_body2d* body, f32 size_x, f32 size_y);

        void update(f32 dt);

        void destroy();

    private:
        b2BodyType game_type_to_b2_type(body_type type);

    private:
        b2Vec2 gravity2d;
        b2World* world2d;

        std::vector<physics_body2d>    bodies2d;
        std::vector<collision_shape2d> shapes2d;
    };
}