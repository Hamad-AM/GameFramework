#include "renderer2d.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

void renderer2d::initialize()
{
    renderer2d::init_sprites();
    renderer2d::init_glyph_bitmaps();
}

void renderer2d::init_sprites()
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
renderer2d::init_glyph_bitmaps()
{
    _text_shader.compile("../assets/shaders/glyph.vs", "../assets/shaders/glyph.fs");

    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    if (FT_New_Face(ft, "../assets/fonts/OpenSans/OpenSans-Regular.ttf", 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    FT_Set_Pixel_Sizes(face, 0, 48);
    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
        std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (ubyte c = 0; c < 128; c++)
    {
        character chars;
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR: Failed to load Glyph" << std::endl;
            continue;
        }

        texture2d texture;
        texture_param param = { texture_wrap::CLAMP_TO_EDGE, texture_filter::BILINEAR, texture_type::DIFFUSE, 0, texture_format::RED, texture_format::RED };
        texture.submit_render(param, face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows);

        chars.texture = texture;
        chars.size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
        chars.bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
        chars.advance = face->glyph->advance.x;

        characters.insert(std::pair<char, character>(c, chars));
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glGenVertexArrays(1, &text_quad_va_);
    glGenBuffers(1, &text_vb_);

    glBindVertexArray(text_quad_va_);

    glBindBuffer(GL_ARRAY_BUFFER, text_vb_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6*4, NULL, GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void renderer2d::begin_sprite(camera2d& camera)
{
    _sprite_shader.bind();
    glm::mat4 projection_view = camera.projection_view();
    _sprite_shader.uniform_matrix4("u_projection_view", projection_view);
}

void renderer2d::draw_sprite(texture2d& texture, glm::vec2 position, glm::vec2 size, f32 rotate, glm::vec3 color)
{
    texture.bind();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0)) * glm::rotate(glm::mat4(1.0f), glm::radians(rotate), glm::vec3(0.0, 0.0, 1.0)) * glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f)); 

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
renderer2d::begin_text(camera2d& camera)
{
    _text_shader.bind();
    glm::mat4 projection = camera.projection();
    _text_shader.uniform_matrix4("projection", projection);
}

void
renderer2d::draw_text(std::string& text, f32 x, f32 y, f32 scale, font_type font_t, glm::vec3 color)
{
    _text_shader.bind();
    _text_shader.uniform_vector3f("textColor", color);
    fonts[font_t].draw(text, x, y, scale, color);
    _text_shader.unbind();
}

void
renderer2d::draw_sprite_single(texture2d& texture, glm::vec2 position, glm::vec2 size, f32 rotate, glm::vec3 color)
{
    _sprite_shader.bind();
    renderer2d::draw_sprite(texture, position, size, rotate, color);
    _sprite_shader.unbind();
}

void
renderer2d::end_sprite()
{
    _sprite_shader.unbind();
    glBindVertexArray(0);
}

void
renderer2d::end_text()
{
    _text_shader.unbind();
    glBindVertexArray(0);
}

void
renderer2d::render_delete()
{
    _sprite_shader.delete_shader();
    font::delete_fonts();
}