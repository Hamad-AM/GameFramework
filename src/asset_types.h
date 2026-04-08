#pragma once
 
#include "common.h"

enum TextureType : u32
{
    ALBEDO,
    NORMAL,
    METALLIC_ROUGHNESS
};

enum TextureFormat : u32
{
    RGBA8,
    RGB8,
    BC7,
    BC6
};

// already packed

// struct Mesh {
//     Vertex* vertices;
//     u32 vertexCount;
//
//     u32* indices;
//     u32 indexCount;
//
//     u32 materialIndex;
// };

struct Texture {
    u8* pixels;
    u32 width;
    u32 height;
    u32 channels;
    u32 sizeBytes;
};

struct Material {
    u32 albedoTexIdx;
    u32 normalTexIdx;
    u32 metallicRoughnessTexIdx;
};

struct Mesh;

struct RenderArray {
    Mesh* meshes;
    u32 meshCount;
    
    Texture* textures;
    u32 textureCount;
    
    Material* materials;
    u32 materialCount;
};

#define MAX_MIPMAP 12

#pragma pack(push, 1)
struct Vertex {
    f32 position[3];
    f32 normal[3];
    f32 uv[2];
    f32 tangent[3];
};

struct TextureHeader
{
    char magic[4] = {'A', 'I', 'M', 'G'};
    u32 width;
    u32 height;
    u32 channels;
    TextureFormat format;
    TextureType type;
    u32 mipmapCount;
};

struct TextureMipmapHeader
{
    u32 mipmapLevel;
    u32 width;
    u32 height;
    u32 numOfBytes;
    u8 offset;
};

struct MaterialHeader {
    u32 albedoIdx;
    u32 normalIdx;
    u32 metallicRoughnessIdx;
};

struct MeshHeader
{
    u64 vertexCount;
    u64 indexCount;
    u32 vertexOffset;
    u32 indexOffset;
    u32 materialIdx;
};

struct SceneHeader
{
    char magic[4] = {'A', 'S', 'C', 'N'};
    u32 meshCount;
    u32 textureCount;
    u32 materialCount;
// How should i know whether offsets are better to be 64 or 32 bit u ints
    u32 meshHeadersOffset;
    u32 materialHeadersOffset;
    u32 textureHeadersOffset;
     
    u64 vertexBufferOffset;
    u64 indexBufferOffset;
    u64 textureDataOffset;
    u64 textureOffsetsOffset;
};

struct LoadedScene {
    MeshHeader* meshes;
    u32 meshCount;
    MaterialHeader* materials;
    u32 materialCount;
    TextureHeader* textures;
    u32 textureCount;

    Vertex* vertices;
    u32* indices;
    u8* textureData;
    u64* textureOffsets;
};
#pragma pack(pop)
