#pragma once

#include <map>

#include <box2d/box2d.h>

#include "../common.h"
#include "../components.h"
#include "../event.h"

#include "physics_body2d.h"
#include "collision_shape2d.h"

class entity;

class collision_event : public event
{
public:
    collision_event(event_type t) : event(t)
    {}

    b32 is_touching;
    entity* entity_a;
    entity* entity_b;
};

class physics_system
{
public:
    physics_system(std::vector<entity>* entities)
        : _entities(entities)
    {}

    physics_system() {}

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

    std::vector<entity>* _entities;

    std::vector<physics_body2d>    bodies2d;
    std::vector<collision_shape2d> shapes2d;
};
