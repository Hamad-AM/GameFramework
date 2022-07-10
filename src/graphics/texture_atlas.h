#pragma once

#include "../common.h"

#include "../gmath.h"

namespace atl
{
    namespace
    {
        struct sub_texture
        {
            ivec2 position;
            ivec2 size;
            u32 vertex_array_handle;
            u32 vertex_buffer_handle;
            u32 index_buffer_handle;
        };
    }

    struct texture_atlas
    {
        std::map<std::string, sub_texture> sub_textures;
        texture2d texture;

    private:
        u32& get_vertex_array(const std::string& name) const
        {
            return sub_textures[name];
        }        
    };
}