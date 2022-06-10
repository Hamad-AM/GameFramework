#pragma once

#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../common.h"
#include "shader.h"
#include "texture.h"
#include "../camera.h"
#include "font.h"

namespace alg
{

    enum font_type
    {
        Montserrat,
        OpenSans,
        Playfair,
        Roboto,
        NUMBER_OF_FONTS,
    };

    class renderer
    {
    public:
        renderer();
        ~renderer() {}

        void
        initialize();

        void begin_sprite(camera2d& camera);

        void draw_sprite(texture2d& texture, vec2 position, vec2 size = vec2(1.0f, 1.0f), f32 rotate = 0.0f, vec3 color = vec3(1.0f));

        void end_sprite();

        void begin_text(camera2d& camera);

        void draw_text(std::string& text, f32 x, f32 y, f32 scale, font_type font_t, vec3& color);
        void draw_text(const char* text, f32 x, f32 y, f32 scale, font_type font_t, vec3& color);

        void end_text();

        void begin_shader(shader& shader);
        void end_shader();

        void begin3D(camera3d& camera);
        void end3D();


        void render_delete();

    private:
        void init_glyph_bitmaps();
        void init_sprites();

        std::vector<font> fonts;

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

}