#pragma once

#include <unordered_map>

#include "common.h"
#include "graphics/shader.h"
#include "graphics/texture.h"

class content
{
public:
    content() {}
    shader load_shader(const char* name, const char* vs, const char* fs);
    texture2d load_texture(const char* file, texture_param params);

    shader*  get_shader(std::string name)    { return &shaders[name]; }
    texture2d* get_texture(std::string name)   { return &textures[name]; }

private:

    std::unordered_map<std::string, texture2d> textures;
    std::unordered_map<std::string, shader> shaders;

};