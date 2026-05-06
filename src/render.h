#pragma once

#include "graphics/shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp>

#include "memory.h"

#include <vector>
#include "asset_types.h"

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

struct SSOLight {
    int type;
    float positionX, positionY, positionZ;
    float directionX, directionY, directionZ;
    float colorX, colorY, colorZ;
    float luminance;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    int isShadowCasting;
    int shadowIndex;
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
    SSOLight* lights;
    u32 numLights;
};

enum DepthFunc {
    LESS_EQUAL,
    DEPTH_NONE,
};

enum BlendFunc {
    ONE_MINUS,
    BLEND_NONE,
};

enum RTextureType {
    TEXTURE_2D,
    TEXTURE_2D_ARRAY,
    TEXTURE_NONE
};

struct Texture2DHandle {
    u32 value;
};

struct TextureArray2DHandle {
    u32 value;
};

struct MaterialHandle {
    u32 value;
};

struct VertexBufferHandle {
    u32 value;
};

struct IndexBufferHandle {
    u32 value;
};

struct VertexAttributeHandle {
    u32 value;
};

struct FrameBufferHandle {
    u32 value;
};

enum MipmapScaling {
    BILINEAR,
    NEAREST_NEIGHBOUR,
    MIPMAP_NONE,
};

enum TextureWrapping {
    BORDER_COLOUR,
    REPEAT,
};

struct RenderTexture2D {
    const char* name;
    u32 width;
    u32 height;
    u8* data;
    TextureFormat format;
    b32 generateMipMaps;
    MipmapScaling scaling;
    TextureWrapping wrap;

    Texture2DHandle handle;
};

struct RenderTextureArray {
    const char* name;
    u32 width;
    u32 height;
    u8* data;
    TextureFormat format;
    b32 generateMipMaps;
    MipmapScaling scaling;
    TextureWrapping wrap;

    TextureArray2DHandle handle;
};

struct RenderDevice {
    RenderTexture2D CreateTexture2D(const char* name, u32 width, u32 height, u8* data, TextureFormat format, b32 generateMipMaps, MipmapScaling scaling, TextureWrapping wrap) {
        u32 textureID;
        RenderTexture2D texture{
            name,
            width,
            height,
            data,
            format,
            generateMipMaps,
            scaling,
            wrap
        };

        glGenTextures(1, &texture.handle.value);
        glBindTexture(GL_TEXTURE_2D, texture.handle.value);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        switch(texture.format)
        {
            case TextureFormat::RGBA8:
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.data);
                break;
            }
            case TextureFormat::BC7:
            {
                assert(false && "No implementation of Compressed Texture BC7");
#if 0
                for (s32 i = 0; i < MAX_MIPMAP; ++i)
                {
                    ImageData& data = image.imageData[i];
                    glCompressedTexImage2D(GL_TEXTURE_2D, i, GL_COMPRESSED_RGBA_BPTC_UNORM, data.width, data.height, 0, data.bytes, data.data);
                }
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#endif
                break;
            }
            default:
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.data);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                glGenerateMipmap(GL_TEXTURE_2D);
                break;
            }
        }

        switch (texture.scaling) {
            case MipmapScaling::BILINEAR:
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                break;
            case MipmapScaling::NEAREST_NEIGHBOUR:
                assert(false && "Not implemented nearest neighbour filtering");
                //TODO: get nearest neighbour scaling done for pixel 3D
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
            default:
                assert(false && "No scaling set");
        }

        if (texture.generateMipMaps) {
                glGenerateMipmap(GL_TEXTURE_2D);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        return texture;
    }

    RenderTextureArray CreateTexture2DArray(s32 count, s32 width, s32 height) {
        RenderTextureArray texture{count, width, height};
        glGenTextures(1, &value);
        glBindTexture(GL_TEXTURE_2D_ARRAY, value);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F,
            width, height, count, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    }
    MaterialHandle CreateMaterial();
    VertexBufferHandle CreateVertexBuffer();
    IndexBufferHandle CreateIndexBuffer();
    VertexAttributeHandle CreateVertexAttrib();
    FrameBufferHandle CreateFrameBuffer();
};

struct Texture {
    GLuint value;
    RTextureType type = TEXTURE_NONE;
    s32 width = 0;
    s32 height = 0;

    void CreateTexture2D() {
    }
};

struct FrameBuffer {
    b32 bDrawBuffer = true;
    b32 bReadBuffer = true;
    b32 enabled = false;
    GLuint frameBuffer;

    // NOTE: This is the gauranteed amount of color attachments;
    constexpr static s32 MaxColourAttachment = 8;
    Texture textureColourAttachment[MaxColourAttachment];
    s32 textureAttachmentCount = -1;

    Texture depthAttachment;

    void CreateFrameBuffer() {
        glGenFramebuffers(1, &frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        enabled = true;
    }
    void StartFrameBuffer() {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        enabled = true;
    }
    void EndFrameBuffer() {
        assert(enabled);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        enabled = false;
    }

    void AttachColor(Texture colorAttachment) {
        assert(enabled && "FrameBuffer Not Bound");
        assert(textureAttachmentCount < MaxColourAttachment && "Too Many Texture Attachments");
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, colorAttachment.value, 0);
        textureColourAttachment[++textureAttachmentCount] = colorAttachment;
    }
    void AttachDepth(Texture depthAttachment) {
        assert(enabled && "FrameBuffer Not Bound");
        assert(depthAttachment.type != TEXTURE_NONE && "Depth Attachment Already Set");
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthAttachment.value, 0);
        this->depthAttachment = depthAttachment;
    }

    void NoDrawBuffer() {
        assert(enabled && "FrameBuffer Not Bound");
        glDrawBuffer(GL_NONE);
    }

    void NoReadBuffer() {
        assert(enabled && "FrameBuffer Not Bound");
        glReadBuffer(GL_NONE);
    }
};

struct RenderState {
    b32 bDepthTesting = false;
    DepthFunc depthFunction = DEPTH_NONE;    
    b32 bBlending = false;
    BlendFunc blendFunction = BLEND_NONE;
    b32 bCullFace = false;
    b32 bBackFace = true;
    b32 bTextureCubeMapSeamless = false;

};

struct RenderTarget {

};


void CompileShaders(RenderData* renderData);

void RenderSetupParameters(RenderData* renderData, u32 render_width, u32 render_height);

void SetupLightsBuffer(RenderData* renderData, Light* lights, u32 numLights);
void UpdateLightsBuffer(RenderData* renderData, u32 numLights);

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
