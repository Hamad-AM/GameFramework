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

#include "gl_types_render.h"

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


enum RTextureFormat {
    RGB8,
    RGBA8,
    RGB16F,
    RGB32F,
    RGBA16F,
    RGBA32F,
    R8,
    RG16F,
    DEPTH16F,
    DEPTH32F,
    COMPRESSED
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
enum MipmapScaling {
    BILINEAR,
    NEAREST_NEIGHBOUR,
    MIPMAP_NONE,
};

enum TextureWrapping {
    BORDER_COLOUR,
    REPEAT,
    CLAMP,
    WRAP_NONE
};

struct RenderTexture2D {
    const char* name;
    u32 width;
    u32 height;
    u8* data;
    RTextureFormat format;
    b32 generateMipMaps;
    MipmapScaling scaling;
    TextureWrapping wrap;

    Texture2DHandle handle;
};

struct RenderTextureArray {
    const char* name;
    s32 count;
    RTextureFormat format;
    u32 width;
    u32 height;
    b32 generateMipMaps;
    MipmapScaling scaling;
    TextureWrapping wrap;

    TextureArray2DHandle handle;
};

struct VertexArrayElement {
    // RenderDataType type;
    s32 size;
    s32 numberOfElements;
};

struct VertexArray {
    s32 numberOfElements = 0;
    u32 stride; 
    VertexArrayElement* elements = nullptr;
    VertexArrayHandle handle;
};

struct Framebuffer {
    // NOTE: This is the gauranteed amount of color attachments;
#define MaxColourAttachment 8
    u32 colors[MaxColourAttachment];
    s32 textureAttachmentCount = 0;

    FramebufferHandle handle;
};

struct TextureCubemap {
    static const u32 FACES = 6;
    u32 width;
    u32 height;
    RTextureFormat format;
    MipmapScaling scaling;
    TextureWrapping wrap;

    TextureCubemapHandle handle;
};

enum DrawState {
    STATIC_DRAW,
    DYNAMIC_DRAW
};

enum BufferType {
    ARRAY,
    INDEX,
};

enum DepthComp {
    DepthComponent,
    DepthComponent24
};

struct RenderBuffer {
    s32 count;
    s32 sizeElement;
    u8* data;
    DrawState state;
    BufferType type;
    BufferHandle handle;
};

struct UniformBuffer {
    u32 handle;
};

struct ShaderStorageObject {
    u32 handle;
};

RenderBuffer CreateBuffer(BufferType type, s32 vertexCount, s32 sizeElement, u8* data, DrawState state);

VertexArray CreateVertexArray(void);
void SetVertexArrayFormat(VertexArray& array, s32 numberOfElements, VertexArrayElement* elements);
void SetVertexArrayVertexBuffer(VertexArray& array, RenderBuffer& vertexBuffer);
void SetVertexArrayIndexBuffer(VertexArray& array, RenderBuffer& indexBuffer);

RenderTexture2D     CreateTexture2D(const char* name, u32 width, u32 height, u8* data, RTextureFormat format, b32 generateMipMaps, MipmapScaling scaling, TextureWrapping wrap);
RenderTextureArray  CreateTexture2DArray(s32 count, RTextureFormat format, u32 width, u32 height, MipmapScaling scaling, TextureWrapping wrap);
TextureCubemap      CreateTextureCubemap(u32 width, u32 height, RTextureFormat format, MipmapScaling scaling, TextureWrapping wrap);
void GenerateMipmap(RenderTexture2D& texture);
void GenerateMipmap(RenderTextureArray& texture);
void GenerateMipmap(TextureCubemap& texture);

Shader CreateShader(const char* vert, const char* frag);
UniformBuffer CreateUniformBuffer(u32 size, DrawState state);
void SetUniformSlot(UniformBuffer& uniform, u32 slot);

Framebuffer CreateFramebuffer(void);
RenderBufferHandle CreateRenderbuffer(u32 width, u32 height, DepthComp depthComp);
void FramebufferAttachColor(Framebuffer& framebuffer, RenderTexture2D& colorAttachment);
void FramebufferAttachDepth(Framebuffer& framebuffer, RenderTexture2D& depthAttachment);
void FramebufferAttachDepth(Framebuffer& framebuffer, RenderTextureArray& depthAttachment);
void FramebufferDrawAttachments(Framebuffer& framebuffer);
void FramebufferNoDrawBuffer(Framebuffer& framebuffer);
void FramebufferNoReadBuffer(Framebuffer& framebuffer);
void BindFramebuffer(Framebuffer& framebuffer, u32 width, u32 height);
void DeleteFramebuffer(Framebuffer& framebuffer);


union RenderResource {
    RenderTexture2D texture2D;
    TextureCubemap cubemap;
    RenderTextureArray textureArray;
};

struct Pipeline {
    b32 bDepthTesting = false;
    DepthFunc depthFunction = DEPTH_NONE;    
    b32 bBlending = false;
    BlendFunc blendFunction = BLEND_NONE;
    b32 bCullFace = false;
    b32 bBackFace = true;
    b32 bTextureCubeMapSeamless = false;
};

void SetRenderPipeline(Pipeline& pipeline);

struct ShaderHandle {
    u32 handle;
};

union Uniform {
    UniformBuffer ubo;
    ShaderStorageObject sso;
};

struct PointLight {
    glm::vec3 position;
    f32 attentuation;
    u32 index;
};

struct RenderMaterial {
    u32 albedoIdx;
    u32 metallicRoughnessIdx;
    u32 normalMapIdx;
};

struct RenderMesh {
    const char* name;
    VertexArray vao;
    RenderBuffer vbo;
    RenderBuffer ebo;
    RenderMaterial material;
};

struct RenderPassDesc {
    Pipeline pipeline;
    Framebuffer renderTarget;
    RenderBufferHandle renderBuffer;
    DepthComp depthComp;
    Shader shader;
    Uniform uniforms[8];
    u32 width;
    u32 height;

    static RenderPassDesc Create(u32 width, u32 height, DepthComp depthComp);
    void SetShader(const char* vs, const char* fs);
    void SetSize(u32 width, u32 height);
    void AttachColor(RenderTexture2D& texture2D);
    void AttachColor(RenderTextureArray& textureArray);
    void AttachColor(TextureCubemap& cubemap);
    void AttachDepth(RenderTexture2D& texture2D);
    void AttachDepth(RenderTextureArray& textureArray);
    void AttachUniformBuffer(u32 slot, u32 size, DrawState drawState);
    void ResetAttachments();
    void ResetColorAttachments();
    void Uniform();
};

struct RenderPass {
    Framebuffer framebuffer;
    Shader shader;
    //TODO: This is for mainly debugging
    RenderPassDesc desc;

    void Bind(RenderTexture2D& texture2D);
    void Bind(RenderTextureArray& textureArray);
    void Bind(TextureCubemap& cubemap);
    void BindPipeline();
    void Execute();
};

RenderPass CreateRenderPass(RenderPassDesc& desc);

struct RenderGraph {
    RenderTexture2D gPosition;
    RenderTexture2D gAlbedo;
    RenderTexture2D gNormal;
    RenderTexture2D gMetallicRoughness;
    RenderPass gBuffer;
    void AddGBufferPass();

    RenderTextureArray cascadeShadowDepthMaps;
    u32 shadowMapResolution = 4096;
    s32 cascadeCount = 4;
    RenderPass cascadeShadowmapPass;
    void AddShadowmapPass();


#define MaxPointLights 64
#define MaxShadowCastPointLights 8

    TextureCubemap pointShadowDepthmaps[MaxShadowCastPointLights];
    u32 castingPointLightCount;
    u32 pointShadowResolution = 256;
    RenderPass pointShadowmapPass;
    void AddPointShadowmapPass(PointLight* lights, u32 lightCount);

    RenderTexture2D ssaoColor;
    RenderTexture2D ssaoBlur;
    RenderTexture2D noiseTexture;
    glm::vec3 ssaoKernel[64];
    RenderPass ssaoPass;
    RenderPass ssaoBlurPass;
    void AddScreenSpaceAmbientOcclusion();


    TextureCubemap skyboxCubemap;
    TextureCubemap irradianceMap;
    TextureCubemap prefilterMap;
    void InitIBL(const char* file);

    RenderTexture2D brdfLUT;  

    u32 screenWidth;
    u32 screenHeight;

    RenderTexture2D* textures;
    u32 textureCount;
    RenderMesh* meshes;
    u32 meshCount;
    RenderMaterial* materials;
    u32 materialCount;

    void DrawToCube(struct RenderCube& cube);
    void DrawToQuad(struct RenderQuad& quad);
    void DrawMesh(RenderMesh& renderMesh);

    void DrawFrame(glm::mat4 model, Camera3D& camer);
};

void AddGBufferPass(RenderGraph& r);
void SetupShadowMapPass(RenderGraph& r);
void SetupPointShadowMapPass(RenderGraph& r, Light* lights, u32 numLights);

float lerp(float a, float b, float f);
