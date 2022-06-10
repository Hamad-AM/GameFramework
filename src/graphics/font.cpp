#include "font.h"

#include <iostream>

namespace alg
{
    //static font::font_library = nullptr;
    FT_Library font::font_library = nullptr;

    void
    font::delete_fonts()
    {
        FT_Done_FreeType(font::font_library);
    }

    font::font(const char* path, u32 size)
        : _path(path), _size(size), _renderable(false)
    {
        if (font::font_library == NULL)
        {
            if (FT_Init_FreeType(&font::font_library))
                std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        } 

        if (FT_New_Face(font::font_library, path, 0, &_face))
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        FT_Set_Pixel_Sizes(_face, 0, _size);
        if (FT_Load_Char(_face, 'X', FT_LOAD_RENDER))
            std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
        
        for (ubyte c = 0; c < 128; c++)
        {
            character chars;
            if (FT_Load_Char(_face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR: Failed to load Glyph" << std::endl;
                continue;
            }

            chars.size = ivec2(_face->glyph->bitmap.width, _face->glyph->bitmap.rows);
            chars.bearing = ivec2(_face->glyph->bitmap_left, _face->glyph->bitmap_top);
            chars.advance = _face->glyph->advance.x;

            _characters.insert(std::pair<char, character>(c, chars));
        }
    }

    font::~font()
    {
        FT_Done_Face(_face);
    }

    void
    font::submit_render()
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (u32 c = 0; c < 128; c++)
        {
            character& chars = _characters[c];

            texture2d texture;
            texture_param param = { texture_wrap::CLAMP_TO_EDGE, texture_filter::BILINEAR, texture_type::DIFFUSE, 0, texture_format::RED, texture_format::RED };
            texture.submit_render(param, _face->glyph->bitmap.buffer, _face->glyph->bitmap.width, _face->glyph->bitmap.rows);

            chars.texture = texture;
        }

        glGenVertexArrays(1, &_va);
        glGenBuffers(1, &_vb);

        glBindVertexArray(_va);

        glBindBuffer(GL_ARRAY_BUFFER, _vb);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6*4, NULL, GL_DYNAMIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        _renderable = true;
    }

    void
    font::draw(const char* text, f32 x, f32 y, f32 scale, vec3 color)
    {
        glBindVertexArray(_va);

        for (u32 i = 0;
            text[i] != '\0';
            ++i)
        {
            character ch = _characters[text[i]];
            f32 xpos = x + ch.bearing.x * scale;
            f32 ypos = y - (ch.size.y - ch.bearing.y) * scale;

            f32 width = ch.size.x * scale;
            f32 height = ch.size.y * scale;
            float vertices[6][4] = {
                { xpos, ypos + height, 0.0f, 0.0f },
                { xpos, ypos, 0.0f, 1.0f },
                { xpos + width, ypos, 1.0f, 1.0f },
                { xpos, ypos + height, 0.0f, 0.0f },
                { xpos + width, ypos, 1.0f, 1.0f },
                { xpos + width, ypos + height, 1.0f, 0.0f }
            };

            ch.texture.bind();
            
            glBindBuffer(GL_ARRAY_BUFFER, _vb);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            x += (ch.advance >> 6) * scale;
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}