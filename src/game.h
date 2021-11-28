#ifndef GAME_HH
#define GAME_HH

#include "common.h"
#include "graphics/sprite_renderer.h"
#include "graphics/camera.h"
#include "input.h"

class game
{
public:
game();

void initialize(input_system *input_system);
void update(f32 game_time);
void draw(f32 game_time);
void close();

private:
    sprite_renderer sprite_render;
    texture2d texture;
    shader s;
    camera cam;
    glm::vec3 camera_position;
    input_system* input;
};

#endif