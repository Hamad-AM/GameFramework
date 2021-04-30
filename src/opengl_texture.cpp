#include "opengl_texture.h"

#include <glad/glad.h>
#include <stb_image.h>

OpenGLTexture2D::OpenGLTexture2D(const char* file_path, TextureFilter filter, TextureWrap wrapping, TextureType type)
    : Texture2D(file_path, filter, wrapping, type)
{
}

void
OpenGLTexture2D::initialize(u32 slot)
{
    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, OpenGLTexture2D::ConvertToGLFilter(filter()));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, OpenGLTexture2D::ConvertToGLFilter(filter()));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, OpenGLTexture2D::ConvertToGLWrapping(wrapping()));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, OpenGLTexture2D::ConvertToGLWrapping(wrapping()));

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width(), height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image_data_);
    slot_ = slot;
    glBindTexture(GL_TEXTURE_2D, 0);
    initialized_ = true;
}

GLenum
OpenGLTexture2D::ConvertToGLWrapping(TextureWrap wrap)
{
    switch(wrap)
    {
        case TextureWrap::CLAMP_TO_BORDER:
            return GL_CLAMP_TO_BORDER;
        case TextureWrap::CLAMP_TO_EDGE:
            return GL_CLAMP_TO_EDGE;
        case TextureWrap::MIRRORED_REPEAT:
            return GL_MIRRORED_REPEAT;
        case TextureWrap::REPEAT:
            return GL_REPEAT;
        default:
            return GL_CLAMP_TO_EDGE;
    }
}

GLenum
OpenGLTexture2D::ConvertToGLFilter(TextureFilter filter)
{
    switch(filter)
    {
        case TextureFilter::NEAREST_NEIGHBOR:
            return GL_NEAREST;
        case TextureFilter::BILINEAR:
            return GL_LINEAR;
        default:
            return GL_LINEAR;
    }
}