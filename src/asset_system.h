#pragma once

#include "render.h"

#include "memory.h"

#include <filesystem>
#include <mutex>
#include <queue>


struct ImageData
{
    u8* data{nullptr};
    u64 bytes{0};
    u32 width{0};
    u32 height{0};
    b32 available{false};
};

struct Image
{
    u32 width;
    u32 height;
    std::string filePath;
    std::string name;
    texture_format format;
    TextureType type;
    #define MAX_MIPMAP 12
    ImageData imageData[MAX_MIPMAP]; // For multiple mipmap levels
};

struct LoadImageJob
{
    std::string path;
    MemoryArena* arena;
    std::string name;
    TextureType type;
};

struct AssetSystem
{
    std::mutex arenaMutex;
    MemoryArena arena;
    std::queue<LoadImageJob> jobs;
    std::mutex queueMutex;
    std::condition_variable condition;
    b32 stopLoading = false;

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

void LoadImageFromFileAsync(AssetSystem& assets, Image& image);

void TranscodeKTX2ToCompressedTexture(std::filesystem::path& filePath);

u32 CreateTexture(Image& image);
