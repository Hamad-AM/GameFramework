#ifndef TEXTURE_HH
#define TEXTURE_HH

#include "platform.h"
#include "shader.h"

enum TextureType
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

enum TextureFilter
{
    NEAREST_NEIGHBOR,
    BILINEAR,
};

enum TextureWrap
{
    REPEAT,
    MIRRORED_REPEAT,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
};

class Texture2D
{
public:
    Texture2D(const char* file_path, TextureFilter filter, TextureWrap wrapping, TextureType type);
    
    static void
    LoadTexture2D(Texture2D* texture);

    s32
    width() { return width_; }

    s32
    height() { return height_; }

    TextureWrap
    wrapping() { return wrap_; }

    TextureFilter
    filter() { return filter_; }

protected:
    ubyte* image_data_;

private:
    s32 width_;
    s32 height_;
    s32 number_of_channels_;
    const char* file_path_;
    TextureWrap wrap_;
    TextureFilter filter_;
    TextureType type_;
};

#endif