#pragma once

#include "common.h"

enum TextureType : u32
{
    ALBEDO,
    NORMAL,
    ROUGHNESS_METALLIC
};

enum TextureFormat : u32
{
    RGBA,
    COMPRESSED_RGBA_BC7
};

#define MAX_MIPMAP 12

#pragma pack(push, 1)
struct Image
{
    char magic[4] = {'A', 'I', 'M', 'G'};
    u32 width;
    u32 height;
    TextureFormat format;
    TextureType type;
    u32 mipmapCount;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct ImageMipmapHeader
{
    u32 mipmapLevel;
    u32 width;
    u32 height;
    u32 numOfBytes;
    u8 offset;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Mesh
{
    u64 vertexCount;
    u64 indexCount;
    u32 vertexOffset;
    u32 indexOffset;
    char[256] albedo;
    char[256] metallicRoughness;
    char[256] normalMap;
    char[256] ao;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Model
{
    char magic[4]
    u32 numberOfMeshes;
};
#pragma pack(pop)
