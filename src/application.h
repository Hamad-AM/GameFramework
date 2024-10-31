#pragma once

#include "common.h"
#include "graphics/renderer2d.h"
#include "graphics/texture.h"
#include "camera.h"
#include "input.h"
#include "entity.h"
#include "glfw_window.h"
#include "audio.h"

#include "Ground.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>
#include <vector>
#include <unordered_map>
#include "audio/mini_audio.h"

namespace atl
{
struct Camera3d
{
    glm::mat4 projection;
    glm::quat orientation;
    glm::vec3 position;
};
struct Material
{
    u32 albedoID;
    u32 metallicRoughnessID;
    u32 normalID;
    u32 aoID;
};
struct Image
{
    u32 width;
    u32 height;
    texture_format format;
    texture_type type;
    std::vector<u8> data;
};
struct Mesh
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<u32> indices;
    std::shared_ptr<Image> alebdo;
    std::shared_ptr<Image> metallicRoughness;
    std::shared_ptr<Image> normalMap;
    std::shared_ptr<Image> ao;
};
struct Model
{
    std::vector<Mesh> meshes;
};

class application
{
public:
    application();
    ~application();

    void initialize(u32 screen_width, u32 screen_height);
    void run();
    void update(f32 dt);
    void close();

private:

    u32 screen_width_;
    u32 screen_height_;
    f64 previous_time;
    platform_window* window;

    std::vector<ref<entity>> entities;

    mini_audio_system audio;
    renderer render;

    camera2d camera2d;
    vec3 camera2d_position;
    Camera3d camera;

    Model sponza;
    std::unordered_map<s32, u32> gpu_textures;
    std::unordered_map<s32, std::shared_ptr<Image>> images;
};
}
