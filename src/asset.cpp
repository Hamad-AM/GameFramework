#include "asset.h"

namespace atl
{
    void asset_system::write_game_asset_from_file(asset_type type, const string& file_path, file_type file_t)
    {
        switch(type)
        {
            case asset_type::texture2d:
                return load_png(file_path, file_t);
                break;
            case asset_type::sound:
                return load_wave(file_path, file_t);
                break;
            case asset_type::font:
                return load_font(file_path, file_t);
                break;
            case asset_type::mesh:
                return load_font(file_path, file_t);
                break;
            case asset_type::scene:
                return load_scene(file_path, file_t);
                break;
            case asset_type::tile_map:
                return load_tile_map(file_path, file_t);
                break;
        }
    }

    asset* asset_system::load_png(const std::string& file, file_type type)
    {
        switch(type)
        {
            case file_type::png:
                image png_image = asset_loader::load_png(file);
                break;
        }
    }
    asset* asset_system::load_wave(const std::string& file, file_type type);
    asset* asset_system::load_ogg(const std::string& file, file_type type);
    asset* asset_system::load_tile_map(const std::string& file, file_type type);
    asset* asset_system::load_scene(const std::string& file, file_type type);
    asset* asset_system::load_font(const std::string& file, file_type type);
};