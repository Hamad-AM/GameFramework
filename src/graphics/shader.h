#pragma once

#include "../common.h"

#include "../gmath.h"
    std::string read_file_to_string(const char* path);

    class Shader
    {
    public:
        Shader();
        void delete_shader();

        void bind();

        void unbind();

        u32 program() { return program_; }

        void set_program(u32 program) { program_ = program; }

        void compile(const char* vs, const char* fs);

        void uniform_float      (const char* name, const f32 value);
        void uniform_int        (const char* name, const s32 value);
        void uniform_vector2f   (const char* name, const vec2& value);
        void uniform_vector3f   (const char* name, const vec3& value);
        void uniform_vector4f   (const char* name, const vec4& value);
        void uniform_matrix4    (const char* name, const mat4& value);

    private:
        s32 uniform_location(const char* name);

        u32 program_;

        const char* shader_name_;
    };  

