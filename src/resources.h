#pragma once

#include "common.h"

#include <array>

#define STBI_LOADER 1

#define MAX_IMAGE_WIDTH     4000
#define MAX_IMAGE_HEIGHT    4000

namespace atl
{
    struct image
    {
        u8* data;
        u32 width;
        u32 height;
        u32 number_of_channels;
        const char* file_path[256];
    };

    struct sound
    {
        const char* file_path[256];
    };

    struct model
    {
        const char* file_path[256];
    };

    class res
    {
    public:
        image* load_image(const char* file_path);

    private:

        constexpr u32 MAX_IMAGES = 100;
        constexpr u32 MAX_MODELS = 100;
        constexpr u32 MAX_SOUND = 100;

        image* images[MAX_IMAGES];
        u64 images_size = 0;
    };
}
