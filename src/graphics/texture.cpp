#include "texture.h"

#include <assert.h>

texture2d::texture2d()
{}

void
texture2d::submit_render(u32 width, u32 height, ubyte* data, texture_param param)
{
    width_ = width;
    height_ = height;
    filter_ = param.filter;
    wrap_ = param.wrap;
    internal_format_ = param.internal_format;
    file_format_ = param.file_format;
    
    glGenTextures(1, &handle_);
    glBindTexture(GL_TEXTURE_2D, handle_);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format_, width_, height_, 0, file_format_, GL_UNSIGNED_BYTE, data);
    slot_ = param.slot;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, convert_to_gl_filter(filter_));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, convert_to_gl_filter(filter_));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, convert_to_gl_wrap(wrap_));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, convert_to_gl_wrap(wrap_));

}

void
texture2d::bind()
{
    glActiveTexture(GL_TEXTURE0 + slot_);
    glBindTexture(GL_TEXTURE_2D, handle_);
}

void
texture2d::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}