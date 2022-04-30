#ifndef GAME_HH
#define GAME_HH

#include "common.h"
#include "graphics/renderer2d.h"
#include "graphics/camera.h"
#include "physics/physics_system.h"
#include "input.h"
#include "entity.h"

#include <iostream>

class application
{
public:
    application();
    ~application();

    void initialize();
    void update(f32 dt);
    void draw(f32 dt);
    void close();

private:
    renderer2d renderer;

    camera2d camera;
    vec3 camera_position;

    physics_system physics_system_;
};

#endif