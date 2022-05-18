#pragma once

#include "../common.h"
#include "../event.h"

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
