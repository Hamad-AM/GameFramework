#include "debug.h"

namespace atl
{
    void debugger::start()
    {
        b2_debug_draw.Create();
    }

    void debugger::end()
    {
        b2_debug_draw.Destroy();
    }

    void debugger::draw_collisions(const game_state& state)
    {
        b2_debug_draw.set_proj(state.camera.projection_view());

        const std::vector<ref<entity>>& entities = state.entities;
        const physics_system& physics = state.physics;

        for (ref<entity> ent : entities)
        {
            if (ent->get_physics_body() != nullptr)
            {
                const b2Body* box2d_body = static_cast<const b2Body*>(ent->get_physics_body()->body);
                b2Color c(sett.collision_shape_color.x, sett.collision_shape_color.y, sett.collision_shape_color.z, sett.collision_shape_color.w);
                const b2Transform& transf = box2d_body->GetTransform();
                b2AABB col_shape;
                const b2Fixture* fix = box2d_body->GetFixtureList();
                const b2Shape* s = fix->GetShape();
                auto b = s->GetChildCount();
                s->ComputeAABB(&col_shape, transf, 0);
                col_shape.lowerBound = b2Vec2(physics.to_game_units(col_shape.lowerBound.x), physics.to_game_units(col_shape.lowerBound.y));
                col_shape.upperBound = b2Vec2(physics.to_game_units(col_shape.upperBound.x), physics.to_game_units(col_shape.upperBound.y));
                b2_debug_draw.DrawAABB(&col_shape, c);
            }   
        }
    }

    void debugger::flush()
    {
        return b2_debug_draw.Flush();
    }

}