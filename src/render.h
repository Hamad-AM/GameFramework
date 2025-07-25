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


struct Material
{
    u32 albedoID;
    u32 metallicRoughnessID;
    u32 normalID;
    u32 aoID;
};

#pragma pack(push, 1)
struct Vertex
{
    f32 position[3];
    f32 normal[3];
    f32 uv[2];
    f32 tangents[3];
};
#pragma pack(pop)

struct Mesh
{
    u64 vertexCount;
    std::vector<f32> vertices;
    
    u64 indexCount;
    std::vector<u32> indices;
    std::string albedo;
    std::string metallicRoughness;
    std::string normalMap;
    std::string ao;
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

    std::unordered_map<std::string, u32> gpu_textures;
    u32 render_width;
    u32 render_height;

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
void DrawScene(RenderData* renderData, std::vector<MeshRenderData>& meshRenderData);

void RenderDepthNormalPass(RenderData* renderData, std::vector<MeshRenderData>& meshRenderData, glm::mat4 model, Camera3D& camera);
void RenderSSAOPass(RenderData* renderData, Camera3D& camera);
void RenderShadowMapPass(RenderData* renderData, vec3& lightDirection, std::vector<MeshRenderData>& meshRenderData, glm::mat4& model, Camera3D& camera);
void RenderOmidirectionalShadowMap(RenderData* renderData, std::vector<MeshRenderData>& meshRenderData, mat4& model, Light* lights, u32 numLights);

void RenderScene(RenderData* renderData, std::vector<MeshRenderData>& meshRenderData, glm::mat4 model, Camera3D& camera, glm::vec3 lightPosition);
void RenderEnvironmentMap(RenderData* renderData, Camera3D& camera);
void RenderEnvironmentMap(RenderData* renderData, mat4& view, mat4& projection);

void SetupLightProbe(RenderData* renderData);
void RenderLightProbe(RenderData* renderData, vec3& lightProbePosition, std::vector<MeshRenderData>& meshRenderData, mat4& model, Light* lights, u32 numOfLights);

void SetupGBuffers(RenderData* renderData);
void RenderGBuffer(RenderData* renderData, std::vector<MeshRenderData>& meshRenderData, glm::mat4 model, Camera3D& camera);
void RenderDeferredScene(RenderData* renderData, std::vector<MeshRenderData>& meshRenderData, glm::mat4 model, Camera3D& camera);

u32 LoadSkyBoxTexture(const char* filePath);

mat4 GetLightSpaceMatrix(Camera3D& camera, vec3& lightDirection, f32 nearPlane, f32 farPlane);
void CalculateCascadesLightSpaceMatrices(mat4* matrices, f32* shadowCascadeLevels, u32 numberOfCascades, vec3& lightDirection, Camera3D& camera);
void getFrustumCornersWorldSpace(glm::vec4* frustumCorners, const glm::mat4& proj, const glm::mat4& view);

void RenderBRDFLUT(RenderData* renderData);
void SetupBRDFLUT(RenderData* renderData, EnvironmentMapPass& pass);

void RenderCube(RenderData* renderData);
void RenderQuad(RenderData* renderData);
void RenderSphere(RenderData* renderData);
float lerp(float a, float b, float f);
