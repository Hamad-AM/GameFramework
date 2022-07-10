#pragma once

#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../common.h"
#include "../camera.h"
#include "../gmath.h"

#include "shader.h"
#include "texture.h"
#include "font.h"

namespace atl
{
    typedef vec3 color;

    enum font_type
    {
        Arial,
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

        void begin2d(camera2d& camera);
 
        void draw_sprite(texture2d& texture, vec2& position, const vec2& size = vec2(1.0f, 1.0f), f32 rotation = 0.0f, const color& col = color(1.0f), f32 tiling_factor = 1.0f, const camera2d* cam = nullptr);

        void draw_text(const std::string& text, f32 x, f32 y, f32 scale, font_type font_t, const color& col, const camera2d* cam = nullptr);
        void draw_text(const char* text, f32 x, f32 y, f32 scale, font_type font_t, const color& col, const camera2d* cam = nullptr);

        void end2d();

        void begin3D(camera3d& camera);
        void end3D();


        void render_delete();

    private:
        void init_glyph_bitmaps();
        void init_sprites();

        ref<font> fonts[font_type::NUMBER_OF_FONTS];

        // shaders
        shader _sprite_shader;
        shader _text_shader;

        // vertex_array handles;
        u32 quad_va_;
        u32 text_quad_va_;
        u32 text_vb_;

        friend texture_atlas;
    };

}