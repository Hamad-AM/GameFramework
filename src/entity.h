#pragma once

#include <vector>

#include "event.h"
#include "components.h"
#include "physics/collision_event.h"
#include "physics/collision_shape2d.h"
#include "physics/physics_body2d.h"


namespace atl
{
    class game_state;
    
    class entity
    {
    public:
        entity() {}

        virtual void init(game_state& state) = 0;

        virtual void update(f32 dt, game_state& state) = 0;

        void destroy()
        {
            if (_sprite != nullptr)
                delete _sprite;
        }


        transform& get_transform() { return _transform; }
        vec3& get_position() { return _transform.position; }
        vec2 get_position2d() { return vec2(_transform.position.x, _transform.position.y); }
        sprite* get_sprite() { return _sprite; }
        
        void set_position(vec2 position) { _transform.position = {position.x, position.y, 0}; }
        void set_sprite(sprite* sp) { _sprite = sp; }

        void add_collision_shape(ref<collision_shape2d> shape) { _collision_shape = shape; }

        void add_physics_body(ref<physics_body2d> body) { _physics_body = body; }

        ref<collision_shape2d> get_collision_shape() { return _collision_shape; }
        ref<physics_body2d> get_physics_body() { return _physics_body; }

        virtual void on_collision_enter(collision_event& collision) { return; }
        
    private:
        transform _transform;

        sprite*             _sprite          = nullptr;
        ref<physics_body2d>     _physics_body;
        ref<collision_shape2d>  _collision_shape;
    };
}