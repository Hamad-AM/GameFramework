#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "platform.h"
#include "shader.h"

#include <glm/glm.hpp>

class SpriteRenderer
{
public:
    SpriteRenderer(ref<Shader>& shader);
    ~SpriteRenderer();

    void DrawSprite(ref<Texture2D> texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 0.0f), f32 rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));

private:
    ref<Shader> shader;
    u32 vertex_array;
};

#endif