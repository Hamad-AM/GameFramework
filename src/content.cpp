#include "content.h"

#include <stb_image.h>
#include <iostream>

shader
content::load_shader(const char* name, const char* vs, const char* fs)
{
    std::string sname(name);
    shader shader;
    shader.compile(vs, fs);
    shaders[sname] = shader;
    return shaders[sname];
}

texture2d
content::load_texture(const char* file, texture_param params)
{
    std::string sname(file);
    texture2d texture;
    s32 width, height, channels;
    ubyte* data = stbi_load(file, &width, &height, &channels, 0);
    if (data) {
        texture.submit_render(width, height, data, params);
    }
    else {
        std::cout << "Error: No such texture as " << file << ". " << std::endl; 
        assert(false);
    }
    textures[sname] = texture;
    return textures[sname];
}