#pragma once

#include "render.h"

#include "memory.h"

struct Image
{
    u32 width;
    u32 height;
    texture_format format;
    u8* data;
};

struct AssetSystem
{
    std::unordered_map<s32, Image*> images;
    std::unordered_map<s32, Mesh*> models;
};

namespace tinygltf {
    class Model;
}

b32 LoadGLTFModel(std::string path, tinygltf::Model& model);

Model GltfToModel(AssetSystem& assets, tinygltf::Model& model, std::string& path, std::unordered_map<s32, u32> gpu_textures);

void BatchModel(AssetSystem& assets, Model& model,
                std::vector<MeshRenderData>& batchMeshRenderData,
                std::unordered_map<s32, u32> gpu_textures);

void LoadScene(AssetSystem& assets, std::string path,
               std::vector<MeshRenderData>& batchMeshRenderData,
               std::unordered_map<s32, u32> gpu_textures);

void InitAssetSystem(AssetSystem& assets);


void EncodeImageToKTX2(std::filesystem::path& filePath, const u8* imageData, u32 width, u32 height);

void TranscodeKTX2ToCompressedTexture(std::filesystem::path& filePath);
