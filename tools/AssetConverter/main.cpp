#include <iostream>
#include <filesystem>

// #include "fbx.h"
// #include "png.h"
// #include "gltf.h"

#include "AssetConverter.h"

#define fs std::filesystem

enum FileType
{
    PNG,
    JPG,
    GLTF,
    FBX
};

fs::path gameFolderForFile(fs::path& devPath)
{
    fs::path result;
    int skipped = 0;
    for (const auto& folder : devPath) {
        if (skipped < 2) {
            ++skipped;
            continue;
        }
        result /= folder;
    }

    return fs::path("assets/game_assets") / result;
}

bool ShouldCreateOrUpdateFile(fs::path gameFile, fs::path devFile) {
    bool convertModel = false;
    if (!fs::exists(gameFile)) {
        convertModel = true;
    } else {
        fs::file_time_type devFileTime = fs::last_write_time(devFile);
        fs::file_time_type gameFileTime = fs::last_write_time(gameFile);
        auto timeSinceChange = std::chrono::duration_cast<std::chrono::seconds>(devFileTime - gameFileTime);
        if (devFileTime > gameFileTime) {
            convertModel = true;
        } else {
            convertModel = false;
        }
    }
    return convertModel;
}


void ConvertFile(FileType type, fs::path devFile) {
    fs::path gameFile = gameFolderForFile(devFile);
    gameFile.replace_extension(".aasset");
    bool convertFile = ShouldCreateOrUpdateFile(gameFile, devFile);                    

    if (!convertFile) {
        return;
    }

    switch (type) {
    case FileType::FBX:
        ConvertFBX2Model(gameFile, devFile);
        break;
    case FileType::GLTF:
        ConvertGLTF2Model(gameFile, devFile);
        break;
    case FileType::PNG:
        ConvertPNG2DDS(gameFile, devFile);
        break;
    default:
        break;
    }
}

int main() {
    fs::path currentPath = fs::current_path();
    fs::path gameAssetPath = "assets/game_assets/"; 
    fs::path devAssetPath = "assets/dev_assets/";

    if (!fs::exists(gameAssetPath)) {
        std::cout << "Game asset does not folder exists" << std::endl;
    }

    if (!fs::exists(devAssetPath)) {
        std::cout << "Development asset does not folder exists" << std::endl;
    }

    if (fs::is_empty(devAssetPath)) {
        std::cout << "There are no Assets in the development asset folder" << std::endl;
    } else {
        std::vector<fs::path> devAssetFiles;
        std::vector<fs::path> dirToLook = { devAssetPath };
        
        while (dirToLook.size() > 0) {
            for (const auto& folderEntry : fs::directory_iterator(dirToLook[0])) {
                if (folderEntry.is_directory()) {
                    dirToLook.push_back(folderEntry);
                } else if (folderEntry.is_regular_file()) {
                    devAssetFiles.push_back(folderEntry);
                } else {
                    std::cout << "Not Folder OR File " << folderEntry.path().string() << std::endl;
                }
            }
            dirToLook.erase(dirToLook.begin());
        }

        for (fs::path& file : devAssetFiles) {
            if (file.extension() == ".gltf") {
                ConvertFile(FileType::GLTF, file);
            } else if (file.extension() == ".png") {
                ConvertFile(FileType::PNG, file);
            } else if (file.extension() == ".fbx") {
                ConvertFile(FileType::FBX, file);
            }
        }
    }

    return 0;
}
