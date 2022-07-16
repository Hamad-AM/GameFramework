#pragma once

#include "../common.h"
#include "../components.h"

#include "collision_shape2d.h"

#include <box2d/box2d.h>

namespace atl
{
    enum body_type { static_body = 0, kinematic_body, dynamic_body };

    class physics_body2d : public component<physics_body2d>
    {
    public:
        physics_body2d() {}

        void update(f32 dt) {}

        f32 get_mass();
        f32 get_inertia();
        vec2 get_local_center();
        vec2 get_position();
        f32 get_angle();
        b32 is_awake();
        void set_awake(b32 flag);

        void apply_force(vec2 force, vec2 point);
        void apply_torque(f32 torque);
        void apply_linear_impluse(vec2 impulse, vec2 point);
        void apply_angular_impulse(f32 impulse);

        body_type type = body_type::static_body;
        vec2 position;
        b32 fixed_rotation;
        std::vector<collision_shape2d> shapes;
        b2Body* body;

    };
}