#pragma once

#include "../common.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "math.h"

class font
{
public:
    font(const char* path, u32 size);
    ~font();
    
    void submit_render();
    void draw(std::string& text, f32 x, f32 y, f32 scale, vec3 color);

    u32 get_size() { return _size; }

    static void delete_fonts();

private:
    struct character
    {
        texture2d texture;
        ivec2 size;
        ivec2 bearing;
        u32 advance;
    };

    
    const char* _path;
    u32 _size;
    std::map<char, character> _characters;

    b32 _renderable;
    u32 _vb;
    u32 _va;
    
    static FT_Library font_library;
    FT_Face _face;
};