#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H


#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../common.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"

class renderer2d
{
public:
    renderer2d() {}
    ~renderer2d() {}

    void
    initialize();

    void begin_sprite(camera2d& camera);

    void draw_sprite(texture2d& texture, glm::vec2 position, glm::vec2 size = glm::vec2(1.0f, 1.0f), f32 rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
    void draw_sprite_single(texture2d& texture, glm::vec2 position, glm::vec2 size = glm::vec2(1.0f, 1.0f), f32 rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));

    void end_sprite();

    void begin_text(camera2d& camera);

    void draw_text(std::string text, f32 x, f32 y, f32 scale, glm::vec3 color);

    void draw_text_single(const char* text, f32 x, f32 y, f32 scale, glm::vec3 color);

    void end_text();


    void render_delete();

private:
    void init_glyph_bitmaps();
    void init_sprites();

    struct character {
        texture2d texture;
        glm::ivec2 size;
        glm::ivec2 bearing;
        u32 advance;
    };

private:
    std::map<char, character> characters;

    // shaders
    shader _sprite_shader;
    shader _text_shader;

    // vertex_array handles;
    u32 quad_va_;
    u32 text_quad_va_;
    u32 text_vb_;

    FT_Library ft;
    FT_Face face;
};

#endif