#pragma once

#include "render.h"
#include "memory.h"
#include "asset_types.h"

struct AssetSystem
{

    LoadedScene scene;
    MemoryArena arena;

#if 0
    std::mutex arenaMutex;
    std::queue<LoadImageJob> jobs;
    std::mutex queueMutex;
    std::condition_variable condition;
    b32 stopLoading = false;
#endif
};

void InitAssetSystem(AssetSystem& assets, MemoryArena arena);
void LoadScene(AssetSystem& assets, std::string path);
void FreeLoadedScene(AssetSystem& assets);

