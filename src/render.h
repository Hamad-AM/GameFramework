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

enum RTextureFormat {
    RGB8,
    RGBA8,
    RGB16F,
    RGB32F,
    RGBA16F,
    RGBA32F,
    R8,
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
    VertexArrayElement* elements = nullptr;
    VertexArrayHandle handle;
};

struct Framebuffer {
    // NOTE: This is the gauranteed amount of color attachments;
#define MaxColourAttachment 8
    RenderTexture2D colors[MaxColourAttachment];
    s32 textureAttachmentCount = -1;

    FramebufferHandle fbHandle;

    u32 width;
    u32 height;
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

struct Buffer {
    s32 count;
    s32 sizeElement;
    u8* data;
    DrawState state;
    BufferType type;
    BufferHandle handle;
};

BufferHandle CreateBuffer(BufferType type, s32 vertexCount, s32 sizeElement, u8* data, DrawState state);
VertexArray CreateVertexArray(void);
void SetVertexArrayFormat(VertexArray& array, s32 numberOfElements, VertexArrayElement* elements);

RenderTexture2D     CreateTexture2D(const char* name, u32 width, u32 height, u8* data, RTextureFormat format, b32 generateMipMaps, MipmapScaling scaling, TextureWrapping wrap);
RenderTextureArray  CreateTexture2DArray(s32 count, RTextureFormat format, u32 width, u32 height, MipmapScaling scaling, TextureWrapping wrap);
TextureCubemap      CreateTextureCubemap(u32 width, u32 height, RTextureFormat format, MipmapScaling scaling, TextureWrapping wrap);

Shader CreateShader(const char* vert, const char* frag);

Framebuffer CreateFramebuffer(void);
RenderBufferHandle CreateRenderbuffer(u32 width, u32 height, DepthComp depthComp);
void FramebufferAttachColor(Framebuffer& framebuffer, RenderTexture2D& colorAttachment);
void FramebufferAttachDepth(Framebuffer& framebuffer, RenderTexture2D& depthAttachment);
void FramebufferDrawAttachments(Framebuffer& framebuffer, u32* textures, u32 numberOfTextures);

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

struct ShaderHandle {
    u32 handle;
};

struct RenderPass {
    Pipeline pipeline;
    Framebuffer renderTarget;
    RenderBufferHandle renderBuffer;
    ShaderHandle shader;
    u32 width;
    u32 height;
};

RenderPass CreateRenderPass(u32 width, u32 height, DepthComp depthComp);
void SetShader(RenderPass& pass, ShaderHandle shader);
void AttachColor(RenderPass& pass, RenderTexture2D texture2D);
void AttachColor(RenderPass& pass, RenderTextureArray textureArray);
void AttachColor(RenderPass& pass, TextureCubemap cubemap);
void Bind(RenderPass& pass, RenderTexture2D texture2D);
void Bind(RenderPass& pass, RenderTextureArray textureArray);
void Bind(RenderPass& pass, TextureCubemap cubemap);
void DrawToCube(RenderPass& pass, struct RenderCube& cube);
void DrawToQuad(RenderPass& pass, struct RenderQuad& quad);
void DrawMesh(RenderPass& pass, RenderMesh& renderMesh);

struct RenderGraph {
    RenderTexture2D gPosition;
    RenderTexture2D gAlbedo;
    RenderTexture2D gNormal;
    RenderTexture2D gMetallicRoughness;
    RenderPass gBuffer;

    RenderTextureArray cascadeShadowDepthMaps;
    u32 shadowMapResolution = 4096;
    s32 cascadeCount = 4;

#define MaxPointLights 64
#define MaxShadowCastPointLights 8

    TextureCubemap pointShadowDepthmaps[MaxShadowCastPointLights];
    u32 castingPointLightCount;
    u32 pointShadowResolution = 256;

    RenderTexture2D ssaoColor;
    RenderTexture2D ssaoBlur;
    RenderTexture2D noiseTexture;
    Framebuffer ssaoTarget;
    Framebuffer ssaoBlurTarget;

    TextureCubemap environmentCubemap;

    u32 screenWidth;
    u32 screenHeight;
};

void AddGBufferPass(RenderGraph& r);
void SetupShadowMapPass(RenderGraph& r);
void SetupPointShadowMapPass(RenderGraph& r, Light* lights, u32 numLights);

float lerp(float a, float b, float f);
