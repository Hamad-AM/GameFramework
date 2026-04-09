#pragma once

#include "graphics/shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp>

#include "memory.h"

#include <vector>

struct LoadedScene;
struct TextureHeader;

struct Camera3D
{
    glm::mat4 projection;
    glm::quat orientation;
    f32 ratio;
    f32 nearPlane;
    f32 farPlane;
    f32 fov;
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::mat4 view;
    glm::vec3 target;
    f32 speed;
};

enum LightType : s32
{
    None,
    Directional,
    Point,
    Spotlight,
    Area
};

struct Light
{
    LightType type{ LightType::None };
    const char* name;

    vec3 position{ 0 };
    vec3 direction{ 0 };

    vec3 color{ 0 };
    f32 luminance{ 0 };

    f32 constant{ 0 };
    f32 linear{ 0 };
    f32 quadratic{ 0 };

    float cutOff{ 0 };
    s32 isShadowCasting{ 0 };
    s32 pointShadowMapIndex;
};

struct RenderMaterial {
    u32 albedoIdx;
    u32 metallicRoughnessIdx;
    u32 normalMapIdx;
};

struct RenderMesh {
    const char* name;
    u32 numberOfIndices;
    GLuint VBO;
    GLuint VAO;
    GLuint EBO;
    RenderMaterial material;
};

struct RenderTexture {
    const char* name;
    GLuint textureID;
};

struct PointLightShadow
{
    GLuint depthCubemap;
    f32 farPlane;
    u32 pointWidth, pointHeight;
    Light* light;
};

struct ShadowPass
{
    GLuint depthMapFBO;
    GLuint lightDepthmaps;
    glm::mat4 lightSpaceMatrix;
    u32 depthMapResolution;
    GLuint lightMatricesUBO;
    const u32 cascadesCount = 4;
    f32 shadowCascadeLevels[3]; // cascade count - 1

    u32 pointCount{ 0 };
    PointLightShadow pointShadows[4];
    GLuint depthCubemapFBO;
    f32 farPlane;

    Shader depthShader;
    Shader pointDepthShader;

    u32 width, height;
};

struct LightProbePass
{
    u32 width;
    u32 height;
    vec3 position;
    GLuint cubemap;
    GLuint irradianceMap;
    GLuint FBO;
    Shader colorShader;
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

struct DeferredPass
{
    GLuint gBuffer;
    GLuint gAlbedo;
    GLuint gMetallicRoughness;
    GLuint gNormal;
    GLuint gPosition;
    GLuint rboDepth;

    Shader gBufferShader;
    Shader lightingPass;
};

struct ForwardPass
{
    Shader pbrShader;
};

struct RenderData
{
    Shader depthNormalShader;
    Shader textureToScreen;
    Shader UnlitShader;

    RenderTexture* textures;
    u32 textureCount;
    RenderMesh* meshes;
    u32 meshCount;
    RenderMaterial* materials;
    u32 materialCount;

    u32 renderWidth;
    u32 renderHeight;

    ShadowPass shadowPass;
    glm::mat4 lightSpaceMatrix;


    AmbientOcclusionPass SSAOPass;
    EnvironmentMapPass environmentMapPass;
    LightProbePass lightProbePass;

    DeferredPass deferredPass;
    ForwardPass forwardPass;

    GLuint quadVAO = 0;
    GLuint cubeVAO = 0;
    GLuint sphereVAO = 0;
    u32 sphereIndexCount = 0;
    Shader sphereCubemap;

    GLuint lightShaderStorageObject;
    Light* lights;
    u32 numLights;
};


void CompileShaders(RenderData* renderData);

void RenderSetupParameters(RenderData* renderData, u32 render_width, u32 render_height);

void SetupLightsBuffer(RenderData* renderData, Light* lights, u32 numLights);
void UpdateLightsBuffer(RenderData* renderData, Light* lights, u32 numLights);

void SetupShadowMapPass(RenderData* renderData, Light* lights, u32 numLights, u32 shadow_width, u32 shadow_height, Camera3D& camera);
void SetupPointShadowMapPass(RenderData* renderData, Light* lights, u32 numLights, u32 shadow_width, u32 shadow_height);

void SetupSSAOPass(RenderData* renderData);

void SetupEnvironmentCubeMap(RenderData* renderData);
void StartDepthNormalPass(RenderData* renderData);
void UseDepthNormalShader(RenderData* renderData, glm::mat4 model, Camera3D& camera);
void DrawScene(RenderData* renderData);
void RenderBindPBRTextures(u32 startingTextureSlot, u32 uniformSlot, Shader* shader, RenderMesh* renderMesh, RenderData* renderData);

void RenderDepthNormalPass(RenderData* renderData, glm::mat4 model, Camera3D& camera);
void RenderSSAOPass(RenderData* renderData, Camera3D& camera);
void RenderShadowMapPass(RenderData* renderData, vec3& lightDirection, glm::mat4& model, Camera3D& camera);
void RenderOmidirectionalShadowMap(RenderData* renderData, mat4& model, Light* lights, u32 numLights);

void RenderScene(RenderData* renderData, glm::mat4 model, Camera3D& camera, glm::vec3 lightPosition);
void RenderEnvironmentMap(RenderData* renderData, Camera3D& camera);
void RenderEnvironmentMap(RenderData* renderData, mat4& view, mat4& projection);

void SetupLightProbe(RenderData* renderData);
void RenderLightProbe(RenderData* renderData, vec3& lightProbePosition, mat4& model, Light* lights, u32 numOfLights);

void SetupGBuffers(RenderData* renderData);
void RenderGBuffer(RenderData* renderData, glm::mat4 model, Camera3D& camera);
void RenderDeferredScene(RenderData* renderData, glm::mat4 model, Camera3D& camera);

u32 LoadSkyBoxTexture(const char* filePath);

mat4 GetLightSpaceMatrix(Camera3D& camera, vec3& lightDirection, f32 nearPlane, f32 farPlane);
void CalculateCascadesLightSpaceMatrices(mat4* matrices, f32* shadowCascadeLevels, u32 numberOfCascades, vec3& lightDirection, Camera3D& camera);
void getFrustumCornersWorldSpace(glm::vec4* frustumCorners, const glm::mat4& proj, const glm::mat4& view);

void RenderBRDFLUT(RenderData* renderData);
void SetupBRDFLUT(RenderData* renderData, EnvironmentMapPass& pass);

void UploadSceneToGPU(LoadedScene& scene, RenderData* renderData, MemoryArena* arena);
RenderTexture CreateTexture(TextureHeader assetTexture, u8* textureData);

void RenderCube(RenderData* renderData);
void RenderQuad(RenderData* renderData);
void RenderSphere(RenderData* renderData);
float lerp(float a, float b, float f);
