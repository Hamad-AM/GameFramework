#pragma once

#include <vector>

#include "event.h"
#include "components.h"
#include "graphics/renderer2d.h"
#include "audio.h"

namespace atl
{
    class game_state;
    
    class entity
    {
    public:
        entity() {}

        virtual void init(renderer* render, audio_system* audio) = 0;

        virtual void update(f32 dt) = 0;

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

        
    private:
        transform _transform;

        sprite* _sprite = nullptr;
    };
}