#pragma once

#include "../common.h"

#include "shader.h"
#include "texture.h"

namespace atl
{
    class render_device
    {
    public:
        virtual void initialize() = 0;

        virtual void compile_shaders(const char* file) = 0;
        virtual void use_shader(ref<shader> shader) = 0;
        
        virtual void add_vertex_array() = 0;
        virtual void use_vertex_array() = 0;
        virtual void remove_vertex_array() = 0;
        
        virtual void add_texture() = 0;
        virtual void use_texture() = 0;
        virtual void remove_texture() = 0;

    private:
    };
}
