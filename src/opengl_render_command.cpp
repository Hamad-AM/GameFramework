#include "render_command_opengl.h"

void
OpenGLRenderCommand::BeginDraw2D(Camera* camera)
{
    camera_ = camera;
    camera_->view = translate(camera->position());
    camera_->set_projection_view(camera->projection() * camera->view());
}

void
OpenGLRenderCommand::DrawSprite(Sprite* sprite, v3* position)
{
    OpenGLTexture2D* texture = (OpenGLTexture2D*)sprite->texture();
    if (!texture->initialized)
    {
        texture->initialize();
        sprite->InitRender();
    }
    
    glActiveTexture(GL_TEXTURE0 + texture->slot());
    glBindTexture(GL_TEXTURE_2D, texture->id());

    glBindVertexArray(sprite->id());

    glUseProgram(texture->GetShaderProgram());

    u32 location = glGetUniformLocation(shader->program, "u_texture");
    if (location == -1)
        assert(true);
    glUniform1i(location, texture->slot());


    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    glBindVertexArray(0);
}

void
OpenGLRenderCommand::DeleteTexture2D(Texture2D* texture)
{}