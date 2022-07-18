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

        ref<physics_body2d> create_body2d(entity* entity, f32 x, f32 y, f32 rotation, body_type type);
        ref<collision_shape2d> add_shape2d(ref<physics_body2d> body, f32 size_x, f32 size_y);

        void update(f32 dt);

        void destroy();

        inline f32 to_box2d_units(f32 p) const { return p / pixels_per_meter; };
        inline f32 to_game_units(f32 p) const { return p * pixels_per_meter; }

    private:
        b2BodyType game_type_to_b2_type(body_type type);


    private:
        b2Vec2 gravity2d;
        b2World* world2d;
        const u32 pixels_per_meter = 128;

        std::vector<ref<physics_body2d>>    bodies2d;
        std::vector<ref<collision_shape2d>> shapes2d;
    };
}