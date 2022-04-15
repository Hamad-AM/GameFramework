#pragma once

#include "entity_manager.h"
#include "system.h"


class world
{
public:

private:
    entity_manager manager;
    std::vector<system_base> systems;
}