#include "physics_system.h"
#include "collision_event.h"

namespace atl
{
    void
    physics_system::initialize2d()
    {
        gravity2d = b2Vec2(0.0f, -9.8f);
        world2d = new b2World(gravity2d);
    }

    ref<physics_body2d>
    physics_system::create_body2d(entity* entity, f32 x, f32 y, f32 rotation, body_type type)
    {
        physics_body2d body;
        b2BodyDef body_def;
        body.type           = type;
        body.fixed_rotation = false;

        body_def.type                  = game_type_to_b2_type(body.type);
        body_def.angle                 = rotation;
        body_def.fixedRotation         = body.fixed_rotation;
        body_def.position.Set(x, y);
        body_def.userData.pointer      = (uintptr_t)entity;

        body.body           = world2d->CreateBody(&body_def);

        bodies2d.push_back(make_ref<physics_body2d>(body));

        return bodies2d[bodies2d.size()-1];
    }

    ref<collision_shape2d>
    physics_system::add_shape2d(ref<physics_body2d> body, f32 size_x, f32 size_y)
    {
        collision_shape2d shape;
        b2PolygonShape pshape;
        shape.size = {size_x, size_y};
        pshape.SetAsBox(shape.size.x, shape.size.y);

        b2FixtureDef fixture_def;
        fixture_def.shape                = &pshape;
        fixture_def.density              = shape.density;
        fixture_def.friction             = shape.friction;
        fixture_def.restitution          = shape.restitution;
        fixture_def.restitutionThreshold = shape.restitution_threshold;
        fixture_def.filter.categoryBits  = shape.category;
        fixture_def.filter.maskBits      = shape.mask;
        fixture_def.isSensor             = shape.trigger;

        static_cast<b2Body*>(body->body)->CreateFixture(&fixture_def);

        shapes2d.push_back(make_ref<collision_shape2d>(shape));

        return shapes2d[shapes2d.size()-1];
    }

    void
    physics_system::update(f32 dt)
    {
        s32 velocity_iterations = 6;
        s32 position_iterations = 2;
        constexpr f32 ts = 1.0f / 60.0f;
        world2d->Step(dt, velocity_iterations, position_iterations);

        for (ref<physics_body2d> pb : bodies2d)
        {
            b2Body* body = static_cast<b2Body*>(pb->body);
            pb->position = { body->GetPosition().x, body->GetPosition().y};
            entity* e = (entity*)body->GetUserData().pointer;
            e->set_position(pb->position);
        }

        for (b2Contact* c = world2d->GetContactList(); c; c = c->GetNext())
        {
            b2Fixture* fixtureA = c->GetFixtureA();
            b2Body* bodyA = fixtureA->GetBody();
            entity* entity_a = (entity*)bodyA->GetUserData().pointer;

            b2Fixture* fixtureB = c->GetFixtureB();
            b2Body* bodyB = fixtureB->GetBody();
            entity* entity_b = (entity*)bodyB->GetUserData().pointer;

            collision_event e{event_type::collision_enter};
            e.entity_a = entity_a;
            e.entity_b = entity_b;
            e.is_touching = c->IsTouching();

            entity_a->on_collision_enter(e);
            entity_b->on_collision_enter(e);
        }

        world2d->ClearForces();
    }


    void
    physics_system::destroy()
    {
        delete world2d;
    }

    b2BodyType
    physics_system::game_type_to_b2_type(body_type type)
    {
        switch (type)
        {
            case body_type::static_body: return b2_staticBody;
            case body_type::kinematic_body: return b2_kinematicBody;
            case body_type::dynamic_body: return b2_dynamicBody;
        }
        return b2_staticBody;
    }
}