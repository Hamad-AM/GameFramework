#pragma once

#include "../common.h"

#include <glad/glad.h>

namespace atl
{
    enum texture_type
    {
        DIFFUSE,
        SPECULAR,
        NORMAL,
        BUMP,
        METAL,
        AO,
        ROUGHNESS,
        HEIGHT,
    };

    enum texture_filter
    {
        NEAREST_NEIGHBOR,
        BILINEAR,
    };

    enum texture_format
    {
        RED,
        RGB,
        RGBA
    };

    enum texture_wrap
    {
        REPEAT,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER,
    };

    struct texture_param
    {
        texture_wrap wrap;
        texture_filter filter;
        texture_type type;
        s32 slot;
        texture_format internal_format;
        texture_format file_format;
    };

    class texture2d 
    {
    public:
        texture2d();
        
        void create(const char* file, texture_param param);

        void submit_render(texture_param param);
        void submit_render(texture_param param, ubyte* data, s32 width, s32 height);

        s32 width() { return width_; }

        s32 height() { return height_; }
        
        texture_wrap wrap() { return wrap_; }

        texture_filter filter() { return filter_; }

        void bind();

        void unbind();

        s32 slot() { return slot_; }
        u32 handle() { return handle_; }

    protected:
        ubyte* image_data_;

    private:
        s32 width_;
        s32 height_;
        s32 number_of_channels_;
        
        const char* filepath_;
        
        texture_format internal_format_;
        texture_format file_format_;
        texture_wrap wrap_;
        texture_filter filter_;
        texture_type type_;

        u32 handle_;
        s32 slot_;

    private:

    GLenum
    convert_to_gl_wrap(texture_wrap wrap)
    {
        switch(wrap)
        {
            case texture_wrap::CLAMP_TO_BORDER:
                return GL_CLAMP_TO_BORDER;
            case texture_wrap::CLAMP_TO_EDGE:
                return GL_CLAMP_TO_EDGE;
            case texture_wrap::MIRRORED_REPEAT:
                return GL_MIRRORED_REPEAT;
            case texture_wrap::REPEAT:
                return GL_REPEAT;
            default:
                return GL_CLAMP_TO_EDGE;
        }
    }

    GLenum
    convert_to_gl_filter(texture_filter filter)
    {
        switch(filter)
        {
            case texture_filter::NEAREST_NEIGHBOR:
                return GL_NEAREST;
            case texture_filter::BILINEAR:
                return GL_LINEAR;
            default:
                return GL_LINEAR;
        }
    }

    GLenum
    convert_to_gl_format(texture_format format)
    {
        switch(format)
        {
            case texture_format::RGB:
                return GL_RGB;
            case texture_format::RGBA:
                return GL_RGBA;
            case texture_format::RED:
                return GL_RED;
            default:
                return GL_RGB;
        }
    }
    };

}