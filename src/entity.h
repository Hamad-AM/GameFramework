#pragma once

#include <vector>

#include "components.h"
#include "physics/physics_system.h"
#include "physics/collision_shape2d.h"
#include "physics/physics_body2d.h"
#include "event.h"

class entity
{
public:
    // game_object() {}

    virtual void start() = 0;
    virtual void update(f32 dt) = 0;
    virtual void destroy() = 0;


    transform_component& get_transform() { return _transform; }
    sprite_component* get_sprite() { return _sprite; }
    
    void set_position(vec2 position) { _transform.position = {position.x, position.y, 0}; }
    void set_sprite(sprite_component* sprite) { _sprite = sprite; }

    virtual void on_collision_enter(collision_event collision) { return; }
    
private:
    transform_component _transform;

    sprite_component*   _sprite          = nullptr;
    physics_body2d*     _physics_body    = nullptr;
    collision_shape2d*  _collision_shape = nullptr;
};