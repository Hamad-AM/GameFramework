#include "texture.h"

#include <assert.h>

#include <stb_image.h>

Texture2D::Texture2D(const char* file_path, TextureFilter filter, TextureWrap wrapping, TextureType type)
    : file_path_(file_path), filter_(filter), wrap_(wrapping), type_(type)
{

}

void
Texture2D::LoadTexture2D(Texture2D* texture)
{
    stbi_set_flip_vertically_on_load(1);
    texture->image_data_ = stbi_load(texture->file_path_, &texture->width_, &texture->height_, &texture->number_of_channels_, 0);
    assert(texture->image_data_);
    stbi_image_free(texture->image_data_);
}