#pragma once

#include "common.h"

class component
{
public:

    virtual void draw(f32 dt) { return; }
    virtual void update(f32 dt) { return; }
    virtual void destroy() { return; }

private:
    
};