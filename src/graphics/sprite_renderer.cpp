#include "sprite_renderer.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


void sprite_renderer::initialize(shader* s)
{
    shader_ = s;
    shader_->bind();
    u32 vertex_buffer;

    f32 vertices[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &vertex_array);
    glGenBuffers(1, &vertex_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(vertex_array);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*) 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void sprite_renderer::draw_sprite(camera& cam, texture2d& texture, glm::vec2 position, glm::vec2 size, f32 rotate, glm::vec3 color)
{
    shader_->bind();

    glm::mat4 model(1.0f);

    model = glm::translate(model, glm::vec3(0.5 * size.x, 0.5 * size.y, 0.0));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0, 0.0, 1.0));
    model = glm::translate(model, glm::vec3(-0.5*size.x, -0.5*size.y, 0.0));
    model = glm::scale(model, glm::vec3(size, 1.0f)); 

    shader_->uniform_matrix4("model", model);
    shader_->uniform_int("image", texture.slot());
    shader_->uniform_vector3f("spriteColor", color);
    glm::mat4 projection = cam.projection_view();
    shader_->uniform_matrix4("projection", projection);


    texture.bind();

    glBindVertexArray(vertex_array);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    glBindVertexArray(0);


}