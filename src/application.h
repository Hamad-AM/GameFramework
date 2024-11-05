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
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::mat4 view;
    glm::vec3 target;
    f32 speed;
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
    std::vector<u8> data;
};
struct Mesh
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<u32> indices;
    s32 albedo;
    s32 metallicRoughness;
    s32 normalMap;
    s32 ao;
    std::string name;
};
struct Model
{
    std::vector<Mesh> meshes;
};
struct MeshOffset {
    u32 vertexOffset = 0;
    u32 indexOffset = 0;
    u32 indexCount = 0;
};
struct RenderData
{
    std::vector<f32> vertices;
    std::vector<u32> indices;
    std::vector<MeshOffset> meshOffsets;
    GLuint VBO;
    GLuint VAO;
    GLuint EBO;
    GLuint albedoID;
    GLuint normalMapID;
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
    Model load_model(std::string path);
    void BatchModel(Model& model);
    void UploadDataToGL(RenderData& data);

    void renderQuad();
    float ourLerp(float a, float b, float f);

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
    glm::vec3 lightPosition;
    std::unordered_map<s32, u32> gpu_textures;
    std::unordered_map<s32, std::shared_ptr<Image>> images;
    std::vector<RenderData> meshRenderData;
    Shader shader;
    Shader depthShader;
    Shader depthNormalShader;
    Shader SSAOShader;
    Shader SSAOBlurShader;
    f32 current_position;
    glm::vec2 lastMousePos;
    f32 pitch = 0;
    f32 yaw = 0;

    GLuint depthMapFBO;
    GLuint depthMap;
    glm::mat4 lightSpaceMatrix;

    GLuint quadVAO = 0;
};
}
