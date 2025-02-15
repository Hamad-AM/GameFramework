#include "font.h"

#include <iostream>
#include <filesystem>

    //static font::font_library = nullptr;
    FT_Library font::font_library = nullptr;

    void
    font::delete_font_library()
    {
        FT_Done_FreeType(font::font_library);
    }

    font::font(const char* path, u32 size)
        : _path(path), _size(size), _renderable(false)
    {
        if (font::font_library == nullptr)
        {
            if (FT_Init_FreeType(&font::font_library))
                std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        }

        if (FT_New_Face(font::font_library, path, 0, &_face))
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

        FT_Set_Pixel_Sizes(_face, 0, _size);
        
        if (FT_Load_Char(_face, 'X', FT_LOAD_RENDER))
            std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
        
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (ubyte c = 0; c < 128; c++)
        {
            character chars;
            if (FT_Load_Char(_face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR: Failed to load Glyph" << std::endl;
                continue;
            }

            texture2d texture;
            texture_param param = { texture_wrap::CLAMP_TO_EDGE, texture_filter::BILINEAR, TextureType::ALBEDO, 0, texture_format::RED, texture_format::RED };
            texture.submit_render(param, _face->glyph->bitmap.buffer, _face->glyph->bitmap.width, _face->glyph->bitmap.rows);

            chars.size = ivec2(_face->glyph->bitmap.width, _face->glyph->bitmap.rows);
            chars.bearing = ivec2(_face->glyph->bitmap_left, _face->glyph->bitmap_top);
            chars.advance = _face->glyph->advance.x;
            chars.texture = texture;

            _characters.insert(std::pair<char, character>(c, chars));
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // TEMP Submitting for render
        submit_render();
    }

    font::~font() {}

    void
    font::delete_font_face()
    {
        //FT_Done_Face(_face);
    }

    void font::submit_render()
    {
        glGenVertexArrays(1, &_va);
        glGenBuffers(1, &_vb);
        glGenBuffers(1, &_ib);

        glBindVertexArray(_va);

        // Vertex Buffer
        glBindBuffer(GL_ARRAY_BUFFER, _vb);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4*5, NULL, GL_DYNAMIC_DRAW);

        // Index Buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ib);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * 6, NULL, GL_DYNAMIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        _renderable = true;
    }

    void
    font::draw(const char* text, f32 x, f32 y, f32 scale, vec3 color)
    {
        glActiveTexture(GL_TEXTURE0);
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
            float vertices[4][5] = {
                { xpos, ypos + height, 0.0f, 0.0f, 0.0f }, // bottom left
                { xpos + width, ypos + height, 0.0f, 1.0f, 0.0f }, // bottom right
                { xpos + width, ypos, 0.0f, 1.0f, 1.0f }, // top right
                { xpos, ypos, 0.0f, 0.0f, 1.0f }, // top left
            };

            u32 indices[] = {
                0, 1, 2,
                2, 3, 0
            };

            ch.texture.bind();
            
            glBindBuffer(GL_ARRAY_BUFFER, _vb);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ib);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);

            glBindVertexArray(_va);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            x += (ch.advance >> 6) * scale;
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }