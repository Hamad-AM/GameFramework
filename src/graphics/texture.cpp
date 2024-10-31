 #include "texture.h"

#include <assert.h>
#include <iostream>
#include <stb_image.h>

namespace atl
{
    texture2d::texture2d()
    {}

    void
    texture2d::create(const char* file, texture_param param)
    {
        stbi_set_flip_vertically_on_load(true);
        image_data_ = stbi_load(file, &width_, &height_, &number_of_channels_, 0);
        if (image_data_)
        {
            texture2d::submit_render(param);
        } else {
            std::cout << "Failed to load texture : " << file << std::endl;  
        }
    }

    void
    texture2d::submit_render(texture_param param)
    {
        filter_ = param.filter;
        wrap_ = param.wrap;
        internal_format_ = param.internal_format;
        file_format_ = param.file_format;
        
        GLenum wrap = texture2d::convert_to_gl_wrap(wrap_);
        GLenum filter = texture2d::convert_to_gl_filter(filter_);
        GLenum internal_format = texture2d::convert_to_gl_format(internal_format_);
        GLenum file_format = texture2d::convert_to_gl_format(file_format_);

        glGenTextures(1, &handle_);
        glBindTexture(GL_TEXTURE_2D, handle_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

        glTexImage2D(GL_TEXTURE_2D, 0, file_format, width_, height_, 0, internal_format, GL_UNSIGNED_BYTE, image_data_);
        slot_ = param.slot;
    }

    void
    texture2d::submit_render(texture_param param, ubyte* data, s32 width, s32 height)
    {
        image_data_ = data;
        width_ = width;
        height_ = height;
        texture2d::submit_render(param);
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
}