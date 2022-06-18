#pragma once

#include "../common.h"

#include "gmath.h"

namespace atl
{
    std::string read_file_to_string(const char* path);

    class shader
    {
    public:
        shader();
        void delete_shader();

        void bind();

        void unbind();

        u32 program() { return program_; }

        void set_program(u32 program) { program_ = program; }

        void compile(const char* vs, const char* fs);

        void uniform_float      (const char* name, f32 value);
        void uniform_int        (const char* name, s32 value);
        void uniform_vector2f   (const char* name, vec2& value);
        void uniform_vector3f   (const char* name, vec3& value);
        void uniform_vector4f   (const char* name, vec4& value);
        void uniform_matrix4    (const char* name, mat4& value);

    private:
        s32 uniform_location(const char* name);

        u32 program_;

        const char* shader_name_;
    };  
}
