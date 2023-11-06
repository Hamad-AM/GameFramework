#pragma once

#include "common.h"

#include "asset_loader.h"

#include <unordered_map>

namespace atl
{
    using asset_handle u64;
    using asset_map std::unordered_map<asset_handle, asset>;

    enum asset_type
    {
        image,
        sound,
        font,
        mesh,
        scene,
        tile_map
    };

    enum file_type
    {
        png,
        wav,
        gltf,
        ttf,
        ogg,
        scene
    };

    struct asset
    {
        asset_handle handle;
        asset_type type;
        union {
            loaded_image image;
            loaded_sound sound;
            loaded_scene scene;
            loaded_font font;
            loaded_mesh mesh;
        }
    };

    class asset_system
    {
    public:
        asset_handle load_asset(asset_type type, const std::string& name);
        void unload_asset(asset_handle& handle);
        asset* get_asset(const asset_handle& handle);

        // Asset Pipeline tool functions
        void compile_all_assets();
        void write_game_asset_from_file(asset_type type, const std::string& file_path, asset_config config);

    private:
        asset* load_png(const std::string& file, file_type type);
        asset* load_wave(const std::string& file, file_type type);
        asset* load_ogg(const std::string& file, file_type type);
        asset* load_tiled_tile_map(const std::string& file, file_type type);
        asset* load_atl_scene(const std::string& file, file_type type);
        asset* load_ttf_font(const std::string& file, file_type type);

        asset_map map;
    };
}