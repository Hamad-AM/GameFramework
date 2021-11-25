#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "../common.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"

class sprite_renderer
{
public:
    sprite_renderer() {}
    ~sprite_renderer() {}

    void
    initialize(shader* s);

    void draw_sprite(camera& cam, texture2d& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 0.0f), f32 rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));

private:
    shader* shader_;
    u32 vertex_array;
};

#endif