#ifndef GAME_HH
#define GAME_HH

#include "common.h"
#include "graphics/sprite_renderer.h"
#include "graphics/camera.h"
#include "content.h"

class game
{
public:

game();
void initialize();
void update(f32 game_time);
void draw(f32 game_time);

private:
    sprite_renderer sprite_render;
    texture2d texture;
    shader s;
    camera cam;
    content con;
};

#endif