#ifndef GAME_HH
#define GAME_HH

#include "common.h"
#include "graphics/renderer2d.h"
#include "graphics/camera.h"
#include "physics/physics_system.h"
#include "input.h"
#include "entity.h"

#include <box2d/box2d.h>

#include <iostream>


class bird : public entity
{
public:
    // bird() : physics_object() {}

    void start() override {};
    void update(f32 dt) override
    {
        elapsed_time += -dt;
        if (elapsed_time > jump_cooldown)
        {
            can_jump = true;
        }

        if (input::is_key_down(key::SPACE) && can_jump)
        {
            // b2Body* body = static_cast<b2Body*>(get_body().body);
            // body->ApplyForceToCenter({0.0f, 7.0f}, true);
            can_jump = false;
            elapsed_time = 0.0f;
        }
        // std::cout << "can jump? : " << can_jump << std::endl;
    }
    void destroy() override {};

private:
    b32 can_jump = true;
    f32 elapsed_time = 0.0f;
    f32 jump_cooldown = 0.5f;
};

class background : public entity
{
public:
    void start() override
    {
        starting_x = 0.0f;
        speed = 0.2f;
    }

    void update(f32 dt) override 
    {
        if (get_transform().position.x > -get_sprite()->size.x)
            get_transform().position.x += speed * dt;
        else
            get_transform().position.x = get_sprite()->size.x - 0.01;
    }
    void destroy() override {}

private:
    f32 starting_x;
    f32 speed;
};

// ref<physics_object> create_bird(glm::vec3 start_position);
// ref<background> create_background(glm::vec3 start_position);

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
    glm::vec3 camera_position;

    physics_system physics_system_;
};

#endif