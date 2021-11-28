#pragma once

#include <vector>

#include "texture.h"

class renderer
{

private:
    std::vector<texture2d> textures;
    std::vector<u32> va_handles;
};