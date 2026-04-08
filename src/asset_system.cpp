#include "asset_system.h"

#include <cstdlib>


#include <iostream>
#include <fstream>

void LoadScene(AssetSystem& assets, std::string path)
{
    auto start = TimeNow();

    FILE* file = fopen(path.c_str(), "rb");
    fseek(file, 0, SEEK_END);
    u64 fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    u8* buffer = PushSize(&assets.arena, fileSize);
    size_t bytesRead = fread(buffer, 1, fileSize, file);
    fclose(file);

    SceneHeader* sceneHeader = (SceneHeader*)buffer;
    // should these be pointers / references?
    assets.scene.meshCount = sceneHeader->meshCount;
    assets.scene.materialCount = sceneHeader->materialCount;
    assets.scene.textureCount = sceneHeader->textureCount;

    assets.scene.meshes = (MeshHeader*)(buffer+sceneHeader->meshHeadersOffset);
    assets.scene.materials = (MaterialHeader*)(buffer+sceneHeader->materialHeadersOffset);
    assets.scene.textures = (TextureHeader*)(buffer+sceneHeader->textureHeadersOffset);

    assets.scene.vertices = (Vertex*)(buffer+sceneHeader->vertexBufferOffset);
    assets.scene.indices = (u32*)(buffer+sceneHeader->indexBufferOffset);
    assets.scene.textureData = (u8*)(buffer+sceneHeader->textureDataOffset);
    assets.scene.textureOffsets = (u64*)(buffer+sceneHeader->textureOffsetsOffset);

    auto end = TimeNow();
    auto duration = Timelapse(end - start);
    std::cout << "Loading Scene " << path << " Took : " << duration.count() / 1000 << std::endl;
}

void InitAssetSystem(AssetSystem& assets, MemoryArena arena)
{
    assets.arena = arena;
};

//
// OLD ASYNCHRONOUS JOB SYSTEM
//
#if 0
// Function to convert a string to lowercase
std::string toLowercase(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

// Function to check if a string contains another string (case-insensitive)
bool containsSubstring(const std::string& mainStr, const std::string& subStr) {
    // Convert both strings to lowercase
    std::string lowerMainStr = toLowercase(mainStr);
    std::string lowerSubStr = toLowercase(subStr);

    // Check if the lowercase main string contains the lowercase substring
    return lowerMainStr.find(lowerSubStr) != std::string::npos;
}


void LoadImageFromFileAsync(AssetSystem& assets, Image& image)
{
    LoadImageJob job = {.path = image.filePath, .arena = &assets.arena, .name = image.name, .type = image.type };

    std::lock_guard<std::mutex> lock(assets.queueMutex);

    assets.jobs.push(std::move(job));
    assets.condition.notify_one();
}

void AddImage(AssetSystem& assets, const tinygltf::Image& image, TextureType type, std::filesystem::path& filePath)
{
    const u8* imageData = image.image.data();
    std::string filename = image.name;
    if (assets.images.find(image.name) != assets.images.end()) return;

    filename += ".aasset";

    std::filesystem::path texture_file = filePath / filename;
    assets.images[image.name] = { .filePath = texture_file.string(), .name = image.name, .type = type };
}

b32 DoLoadImageJob(AssetSystem* assets, LoadImageJob& job)
{
    std::unique_lock<std::mutex> lock(assets->queueMutex);
    assets->condition.wait(lock, [assets] { return !assets->jobs.empty() || assets->stopLoading; });
    if (assets->stopLoading && assets->jobs.empty()) return false;
    job = std::move(assets->jobs.front());
    assets->jobs.pop();
    return true;
}


u8* AllocateAsset(AssetSystem* assets, u64& size)
{
    std::lock_guard<std::mutex> lock(assets->arenaMutex);
    u8* buffer = PushSize(&assets->arena, size);
    return buffer;
}

void LoadImageFromFile(AssetSystem* assets, std::string& path, std::string& name)
{
    FILE* file = fopen(path.c_str(), "rb");
    if (!file)
    {
        std::cerr << "Could not read file : " << path << std::endl;
    }

    fseek(file, 0, SEEK_END);
    u64 fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    u8* buffer = AllocateAsset(assets, fileSize);
    size_t bytesRead = fread(buffer, 1, fileSize, file);
    fclose(file);
    //std::cout << "Read file from : " << std::this_thread::get_id() << std::endl;

    assets->images[name] = (Image*)buffer;
}

void ShutDownLoading(AssetSystem& assets)
{
    std::lock_guard<std::mutex> lock(assets.queueMutex);
    assets.stopLoading = true;
    assets.condition.notify_all();
}

void LoadImageFromFileNormal(AssetSystem* assets, std::string& path, std::string& name)
{
    std::string filename = name;
    u8* buffer = nullptr;
    s32 width, height, channels;
    u8* imageDatastb = stbi_load(path.c_str(), &width, &height, &channels, 4);
    u64 size = width * height * 4 * sizeof(u8);
    buffer = AllocateAsset(assets, size);
    memcpy(buffer, imageDatastb, width * height * 4 * sizeof(u8));
    stbi_image_free(imageDatastb);
    
    assets->images[name] = Image{};
    Image& i = assets->images[name];
    i.width = width;
    i.height = height;
    i.format = texture_format::RGBA;
    i.imageData[0].data = buffer;
    i.filePath = path;
    i.type = NORMAL;
}

void LookForWork(int threadId, AssetSystem* assets)
{
    while (true)
    {
        LoadImageJob job;
        if (!DoLoadImageJob(assets, job)) break;
        if (job.type == TextureType::NORMAL)
        {
            LoadImageFromFileNormal(assets, job.path, job.name);
        }
        else
        {
            LoadImageFromFile(assets, job.path, job.name);
        }
    }
}
#endif
