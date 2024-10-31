#include "renderer2d.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

namespace atl
{
    
    renderer::renderer()
    {
        
    }

    void renderer::initialize()
    {
        renderer::init_sprites();
        renderer::init_glyph_bitmaps();
    }

    void renderer::init_sprites()
    {
        _sprite_shader.compile("../assets/shaders/sprite.vs", "../assets/shaders/sprite.fs");

        f32 vertices[] = {
            // positions          // texture coords
            -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, // bottom left
            1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // bottom right
            1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // top right
            -1.0f,  1.0f, 0.0f,   0.0f, 1.0f  // top left 
        };
        
        u32 indices[] = {  
            0, 1, 2 , // first triangle
            2, 3, 0   // second triangle
        };

        u32 VBO, VAO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // // texture coord attribute
        // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        // glEnableVertexAttribArray(2);

        quad_va_ = VAO;
    }

    void
    renderer::init_glyph_bitmaps()
    {
        _text_shader.compile("../assets/shaders/glyph.vs", "../assets/shaders/glyph.fs");

        fonts[font_type::Arial]      = ref<font>(new font("../assets/fonts/Arial/arial.ttf", 48));
        fonts[font_type::Montserrat] = ref<font>(new font("../assets/fonts/Montserrat/Montserrat-Regular.ttf", 48));
        fonts[font_type::OpenSans]   = ref<font>(new font("../assets/fonts/OpenSans/OpenSans-Regular.ttf", 48));
        fonts[font_type::Playfair]   = ref<font>(new font("../assets/fonts/Playfair/PlayfairDisplay-Regular.ttf", 48));
        fonts[font_type::Roboto]     = ref<font>(new font("../assets/fonts/Roboto/Roboto-Regular.ttf", 48));
        
    }

    void renderer::begin2d(camera2d& camera)
    {
        clear();

        _sprite_shader.bind();
        mat4 projection_view = camera.projection_view();
        _sprite_shader.uniform_matrix4("u_projection_view", projection_view);
    }

    void renderer::draw_texture(texture2d& texture, vec2& position, const vec2& size, f32 rotation, const color& col, f32 tiling_factor, const camera2d* cam)
    {
        draw_quad(quad_va_, texture, position, size, rotation, col, tiling_factor, cam);
    }

    void
    renderer::draw_texture(std::string& name, texture_atlas& atlas, vec2& position, const vec2& size, f32 rotation, const color& col, f32 tiling_factor, const camera2d* cam)
    {
        draw_quad(atlas.sub_textures[name].va, atlas.texture, position, size, rotation, col, tiling_factor, cam);
    }

    void
    renderer::draw_quad(u32 vertex_array, texture2d& texture, vec2& position, const vec2& size, f32 rotation, const color& col, f32 tiling_factor, const camera2d* cam)
    {
        _sprite_shader.bind();
        texture.bind();

        if (cam != nullptr)
        {
            const mat4& projection_view = cam->projection_view();
            _sprite_shader.uniform_matrix4("u_projection_view", projection_view); 
        }

        mat4 model = translate(mat4(1.0f), vec3(position.x, position.y, 0.0))
                     * rotate(mat4(1.0f), rad2deg(rotation), vec3(0.0, 0.0, 1.0))
                     * scale(mat4(1.0f), vec3(size.x, size.y, 1.0f)); 

        _sprite_shader.uniform_matrix4("u_model", model);
        _sprite_shader.uniform_int("u_texture", texture.slot());
        _sprite_shader.uniform_vector3f("spriteColor", col);
        _sprite_shader.uniform_float("u_tiling_factor", tiling_factor);


        glBindVertexArray(vertex_array);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        _sprite_shader.unbind();
    }

    void
    renderer::draw_text(const std::string& text, f32 x, f32 y, f32 scale, font_type font_t, const color& col, const camera2d* cam)
    {
        renderer::draw_text(text.c_str(), x, y, scale, font_t, col, cam);
    }

    void
    renderer::draw_text(const char* text, f32 x, f32 y, f32 scale, font_type font_t, const color& col, const camera2d* cam)
    {
        _text_shader.bind();

        _text_shader.uniform_matrix4("projection", cam->projection());
        _text_shader.uniform_vector3f("textColor", col);
        _text_shader.uniform_int("text", 0);
     
        // mat4 model = translate(mat4(1.0f), vec3(x, y, 0));
        // _sprite_shader.uniform_matrix4("u_model", model);
        fonts[font_t]->draw(text, x, y, scale, col);
        _text_shader.unbind();
    }
    
    void
    renderer::end2d()
    {
        _sprite_shader.unbind();
    }

    void
    renderer::render_delete()
    {
        for (s32 i = 0; i < font_type::NUMBER_OF_FONTS; ++i)
        {
            fonts[i]->delete_font_face();
        }
        _sprite_shader.delete_shader();
        font::delete_font_library();
    }

    void
    renderer::clear()
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}