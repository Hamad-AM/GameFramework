#pragma once

#include "render.h"

#include "memory.h"

#include <filesystem>

class ktxTexture;

struct Image
{
    u32 width;
    u32 height;
    texture_format format;
    TextureType type;
    ktxTexture* ktxTexture;
    u32 size;
    u8* data{ nullptr };
};

struct AssetSystem
{
    MemoryArena arena;
    std::unordered_map<std::string, Image> images;
    std::unordered_map<s32, Mesh*> models;
};

namespace tinygltf {
    class Model;
    class Image;
}

b32 LoadGLTFModel(std::string path, tinygltf::Model& model);

Model GltfToModel(AssetSystem& assets, tinygltf::Model& model, std::string& path, std::unordered_map<s32, u32> gpu_textures);

void BatchModel(AssetSystem& assets, Model& model,
                std::vector<MeshRenderData>& batchMeshRenderData,
                std::unordered_map<s32, u32> gpu_textures);

void LoadScene(AssetSystem& assets, std::string path,
               std::vector<MeshRenderData>& batchMeshRenderData,
               std::unordered_map<std::string, u32> gpu_textures);

void InitAssetSystem(AssetSystem& assets, MemoryArena arena);

void EncodeImageToKTX2(std::filesystem::path& filePath, const u8* imageData, u32 width, u32 height);

void LoadImageFromFile(AssetSystem& assets, const tinygltf::Image& image, std::filesystem::path& textures_path);

void TranscodeKTX2ToCompressedTexture(std::filesystem::path& filePath);

u32 CreateTexture(Image& image);
