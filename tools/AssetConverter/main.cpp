#include <iostream>
#include <vector>

#include "AssetConverter.h"

std::filesystem::path gameFolderForFile(std::filesystem::path& devPath)
{
    std::filesystem::path result;
    int skipped = 0;
    for (const auto& folder : devPath) {
        if (skipped < 2) {
            ++skipped;
            continue;
        }
        result /= folder;
    }

    return std::filesystem::path("assets/asset-game") / result;
}

bool ShouldCreateOrUpdateFile(std::filesystem::path gameFile, std::filesystem::path devFile) {
    bool convertModel = false;
    if (!std::filesystem::exists(gameFile)) {
        convertModel = true;
    } else {
        convertModel = true;
        //TODO: Add option to complete rebuild assets through flag
#if 0
        std::filesystem::file_time_type devFileTime = std::filesystem::last_write_time(devFile);
        std::filesystem::file_time_type gameFileTime = std::filesystem::last_write_time(gameFile);
        auto timeSinceChange = std::chrono::duration_cast<std::chrono::seconds>(devFileTime - gameFileTime);
        if (devFileTime > gameFileTime) {
            convertModel = true;
        } else {
            convertModel = false;
        }
#endif
    }
    return convertModel;
}


void ConvertFile(FileType type, std::filesystem::path devFile) {
    std::filesystem::path gameFile = gameFolderForFile(devFile);
    gameFile.replace_extension(".asset");
    bool convertFile = ShouldCreateOrUpdateFile(gameFile, devFile);                    

    if (!convertFile) {
        return;
    }

    switch (type) {
#if 0
    case FileType::FBX:
        ConvertFBX2Model(gameFile, devFile);
        break;
    case FileType::GLTF:
        ConvertGLTF2Model(gameFile, devFile);
        break;
    case FileType::PNG:
        ConvertPNG2DDS(gameFile, devFile);
        break;
#endif
    case FileType::GLB:
        ConvertGLB(gameFile, devFile);
    default:
        break;
    }
}

int main() {
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::path devAssetPath = "assets/asset-source/"; 
    std::filesystem::path gameAssetPath = "assets/asset-game/";

    if (!std::filesystem::exists(gameAssetPath)) {
        std::cout << "Game asset does not folder exists" << std::endl;
    }

    if (!std::filesystem::exists(devAssetPath)) {
        std::cout << "Development asset does not folder exists" << std::endl;
    }

    if (std::filesystem::is_empty(devAssetPath)) {
        std::cout << "There are no Assets in the development asset folder" << std::endl;
    } else {
        std::vector<std::filesystem::path> devAssetFiles;
        std::vector<std::filesystem::path> dirToLook = { devAssetPath };
        
        while (dirToLook.size() > 0) {
            for (const auto& folderEntry : std::filesystem::directory_iterator(dirToLook[0])) {
                if (folderEntry.is_directory()) {
                    dirToLook.push_back(folderEntry);
                } else if (folderEntry.is_regular_file()) {
                    std::cout << "Found file : " << folderEntry.path().filename() << std::endl;
                    devAssetFiles.push_back(folderEntry);
                } else {
                    std::cout << "Not Folder OR File " << folderEntry.path().string() << std::endl;
                }
            }
            dirToLook.erase(dirToLook.begin());
        }

        for (std::filesystem::path& file : devAssetFiles) {
            if (file.extension() == ".gltf") {
                ConvertFile(FileType::GLTF, file);
            } else if (file.extension() == ".png") {
                ConvertFile(FileType::PNG, file);
            } else if (file.extension() == ".fbx") {
                ConvertFile(FileType::FBX, file);
            } else if (file.extension() == ".glb") {
                ConvertFile(FileType::GLB, file);
            }
        }
    }

    return 0;
}
