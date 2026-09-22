#pragma once

#include <filesystem>
#include "../../src/asset_types.h"

enum FileType
{
    PNG,
    JPG,
    GLTF,
    FBX,
    GLB
};

// void ConvertPNG2DDS(fs::path& gameFile, fs::path& file);
// void ConvertGLTF2Model(fs::path& gameFile, fs::path& file);
void ConvertGLB(std::filesystem::path& gameFile, std::filesystem::path& file);
