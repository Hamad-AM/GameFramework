#ifndef SPRITE_HH
#define SPRITE_HH

#include "texture.h"
#include "shader.h"
#include "math.h"

class Sprite
{
    public:
        Sprite(Texture2D* texture, Shader* shader, v3 position = v3(1.0f, 1.0f, 1.0f));

        void
        InitRender();

        Texture2D*
        texture() { return texture_; }

        v3
        position() { return position_; }
        
        Shader*
        shader() { return shader_;}

        u32
        vertex_array() { return vertex_array_; }

        void
        set_position(v3 position) { position_ = position; }

    private:
        u32 sprite_count_;
        Texture2D* texture_;
        v3 position_;
        Shader* shader_;
        u32 vertex_array_;
        v3 color_tint_;
        v2 size_;
};
#endif