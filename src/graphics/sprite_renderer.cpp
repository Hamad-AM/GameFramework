#include "sprite_renderer.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>


void sprite_renderer::initialize()
{
    shader_.compile("texture.vs", "texture.fs");

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

    vertex_array_ = VAO;
}

void sprite_renderer::draw_sprite(camera& cam, texture2d& texture, glm::vec2 position, glm::vec2 size, f32 rotate, glm::vec3 color)
{
    texture.bind();
    shader_.bind();


    glm::mat4 model(1.0f);

    model = glm::translate(model, glm::vec3(0.5 * size.x, 0.5 * size.y, 1.0));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0, 0.0, 1.0));
    // model = glm::translate(model, glm::vec3(position.x, position.y, 0.0));
    model = glm::scale(model, glm::vec3(size.x, size.y, 1.0f)); 

    shader_.uniform_matrix4("model", model);
    shader_.uniform_int("texture1", texture.slot());
    glm::mat4 projection_view = cam.projection_view();
    shader_.uniform_matrix4("projection_view", projection_view);

    glBindVertexArray(vertex_array_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // std::cout << "Vertex Array   : " << vertex_array_ << std::endl;
    // std::cout << "Texture Handle : " << texture.handle() << std::endl;


    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    glBindVertexArray(0);
}

void
sprite_renderer::render_delete()
{
    shader_.delete_shader();
}