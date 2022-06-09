#include "renderer2d.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

namespace alg
{
    void renderer()
    {
        fonts.reserve(font_type::NUMBER_OF_FONTS);
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
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
            0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
            0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
            -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
        };
        
        u32 indices[] = {  
            0, 1, 2 , // first triangle
            2, 3, 0  // second triangle
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

        font m("../assets/fonts/Montserrat/Montserrat-Regular.ttf", 48);
        font o("../assets/fonts/OpenSans/OpenSans-Regular.ttf", 48);
        font pf("../assets/fonts/Playfair/Playfair-Regular.ttf", 48);
        font r("../assets/fonts/Roboto/Roboto-Regular.ttf", 48);
        fonts[font_type::Montserrat] = m;
        fonts[font_type::OpenSans]   = o;
        fonts[font_type::Playfair]   = pf;
        fonts[font_type::Roboto]     = r;
        
    }

    void renderer::begin_sprite(camera2d& camera)
    {
        _sprite_shader.bind();
        mat4 projection_view = camera.projection_view();
        _sprite_shader.uniform_matrix4("u_projection_view", projection_view);
    }

    void renderer::draw_sprite(texture2d& texture, vec2 position, vec2 size, f32 rotate, glm::vec3 color)
    {
        texture.bind();

        mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0)) * glm::rotate(glm::mat4(1.0f), glm::radians(rotate), glm::vec3(0.0, 0.0, 1.0)) * glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f)); 

        _sprite_shader.uniform_matrix4("u_model", model);
        _sprite_shader.uniform_int("u_texture", texture.slot());

        glBindVertexArray(quad_va_);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // std::cout << "Vertex Array   : " << quad_va_ << std::endl;
        // std::cout << "Texture Handle : " << texture.handle() << std::endl;


        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
    }

    void
    renderer::begin_text(camera2d& camera)
    {
        _text_shader.bind();
        glm::mat4 projection = camera.projection();
        _text_shader.uniform_matrix4("projection", projection);
    }

    void
    renderer::begin_shader(shader& shader)
    {
        shader.bind();
    }

    void
    renderer::end_shader()
    {
        glUseProgram(0);
    }

    void
    renderer::draw_text(std::string& text, f32 x, f32 y, f32 scale, font_type font_t, glm::vec3 color)
    {
        _text_shader.uniform_vector3f("textColor", color);
        fonts[font_t].draw(text, x, y, scale, color);
    }

    void
    renderer::end_sprite()
    {
        _sprite_shader.unbind();
        glBindVertexArray(0);
    }

    void
    renderer::end_text()
    {
        _text_shader.unbind();
        glBindVertexArray(0);
    }

    void
    renderer::render_delete()
    {
        _sprite_shader.delete_shader();
        font::delete_fonts();
    }
}