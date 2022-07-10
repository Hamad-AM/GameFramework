#include "resources.h"

namespace atl
{
    image*
    load_image(const char* file_path)
    {
        images[images_size].file_path = file_path;
        
#if STBI_LOADER

        stbi_set_flip_vertically_on_load(true);
#endif
    }
}