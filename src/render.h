#pragma once

#include "graphics/shader.h"
#include "graphics/texture.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>
#include <unordered_map>

struct Camera3D
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

struct MeshRenderData
{
    u32 numberOfIndices;
    GLuint VBO;
    GLuint VAO;
    GLuint EBO;
    GLuint albedoID;
    GLuint metallicRoughnessID;
    GLuint normalMapID;
};

struct ShadowPass
{
    GLuint depthMapFBO;
    GLuint depthMap;
    glm::mat4 lightSpaceMatrix;

    Shader depthShader;

    u32 width, height;
};

struct AmbientOcclusionPass {

    //TODO(): Remove this to a deferred pass
    GLuint depthBuffer, normalBuffer, positionBuffer, depthNormalPassBO;
    GLuint ssaoFBO, ssaoBlurFBO;
    GLuint ssaoColorBuffer, ssaoColorBufferBlur;
    GLuint noiseTexture; 
    std::vector<glm::vec3> ssaoKernel;
    Shader SSAOShader;
    Shader SSAOBlurShader;
};

struct EnvironmentMapPass 
{
    GLuint captureFBO, captureRBO;
    GLuint envCubemap;
    GLuint irradianceMap;
    GLuint prefilterMap;
    GLuint brdfLUTTexture;
    Shader brdfShader;
    Shader equirectangularToCubemapShader;
    Shader prefilterShader;
    Shader irradianceShader;
    Shader backgroundShader;
};

struct RenderData
{
    Shader pbrShader;

    Shader depthNormalShader;
    Shader textureToScreen;

    std::unordered_map<s32, u32> gpu_textures;
    u32 render_width;
    u32 render_height;

    ShadowPass shadowPass;
    glm::mat4 lightSpaceMatrix;

    AmbientOcclusionPass SSAOPass;
    EnvironmentMapPass environmentMapPass;

    GLuint quadVAO = 0;
    GLuint cubeVAO = 0;
};


void CompileShaders(RenderData* renderData);

void RenderSetupParameters(RenderData* renderData, u32 render_width, u32 render_height);


void SetupShadowMapPass(RenderData* renderData, u32 shadow_width, u32 shadow_height);

void SetupSSAOPass(RenderData* renderData);

void SetupEnvironmentCubeMap(RenderData* renderData);
void StartDepthNormalPass(RenderData* renderData);
void UseDepthNormalShader(RenderData* renderData, glm::mat4 model, Camera3D& camera);
void DrawScene(RenderData* renderData, std::vector<MeshRenderData>& meshRenderData);
void RenderDepthNormalPass(RenderData* renderData, std::vector<MeshRenderData>& meshRenderData, glm::mat4 model, Camera3D& camera);
void RenderSSAOPass(RenderData* renderData, Camera3D& camera);
void RenderShadowMapPass(RenderData* renderData, std::vector<MeshRenderData>& meshRenderData, glm::mat4 model, glm::vec3 lightPosition);
void RenderScene(RenderData* renderData, std::vector<MeshRenderData>& meshRenderData, glm::mat4 model, Camera3D& camera, glm::vec3 lightPosition);
void RenderEnvironmentMap(RenderData* renderData, Camera3D& camera);

u32 LoadSkyBoxTexture(const char* filePath);

void RenderBRDFLUT(RenderData* renderData);
void SetupBRDFLUT(RenderData* renderData, EnvironmentMapPass& pass);

void RenderCube(RenderData* renderData);
void RenderQuad(RenderData* renderData);
float lerp(float a, float b, float f);
