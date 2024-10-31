#pragma once

#include "../common.h"
#include "../gmath.h"
#include "texture.h"

#include <map>

namespace atl
{
    struct sub_texture
    {
        ivec2 position;
        u32 va;
        u32 vb;
        u32 ib;
    };

    struct texture_atlas
    {
        std::map<std::string, sub_texture> sub_textures;
        texture2d texture;
        ivec2 cell_size{32, 32};

        void create_sub_texture(std::string& name, ivec2& position);    
    };
}