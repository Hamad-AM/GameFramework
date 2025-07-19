#include "../../src/common.h"
#include "../../src/asset_types.h"

#include <filesystem>
#include <iostream>
#include <windows.h>

#define fs std::filesystem

struct DDS_PIXELFORMAT {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwFourCC;
    DWORD dwRGBBitCount;
    DWORD dwRBitMask;
    DWORD dwGBitMask;
    DWORD dwBBitMask;
    DWORD dwABitMask;
};

typedef struct {
    DWORD           dwSize;
    DWORD           dwFlags;
    DWORD           dwHeight;
    DWORD           dwWidth;
    DWORD           dwPitchOrLinearSize;
    DWORD           dwDepth;
    DWORD           dwMipMapCount;
    DWORD           dwReserved1[11];
    DDS_PIXELFORMAT ddspf;
    DWORD           dwCaps;
    DWORD           dwCaps2;
    DWORD           dwCaps3;
    DWORD           dwCaps4;
    DWORD           dwReserved2;
} DDS_HEADER;

#include <dxgiformat.h>
#include <d3d10.h>
#include "../../src/common.h"
#include "../../src/asset_types.h"
typedef struct {
    DXGI_FORMAT              dxgiFormat;
    D3D10_RESOURCE_DIMENSION resourceDimension;
    UINT                     miscFlag;
    UINT                     arraySize;
    UINT                     miscFlags2;
} DDS_HEADER_DXT10;

struct DDSFile {
    DWORD dwmagic;
    DDS_HEADER header;
    DDS_HEADER_DXT10 header10;
    BYTE* bdata;
    BYTE* bdata2;
};

void ConvertPNG2DDS(fs::path& gameFile, fs::path& devFile) {
    fs::path texturePath = devFile.parent_path();
    texturePath = devFile;
    texturePath.replace_extension(".dds");
    std::string commandCall = "nvcompress -color -alpha -bc7 -fast -m " + devFile.string() + " " + texturePath.string();
    s32 result = system(commandCall.c_str());

    FILE* file = fopen(texturePath.string().c_str(), "rb");
    if (!file)
    {
        std::cout << "Could not read file : " << texturePath.string() << std::endl;
        return;
    }

    fseek(file, 0, SEEK_END);
    u64 fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    u8* buffer = (u8*)malloc(fileSize);
    size_t bytesRead = fread(buffer, 1, fileSize, file);
    fclose(file);
    //std::cout << "Read file from : " << std::this_thread::get_id() << std::endl;

    DDSFile* ddsImage = (DDSFile*)buffer;

    u32 width = (u32)ddsImage->header.dwWidth;
    u32 height = (u32)ddsImage->header.dwHeight;
    u64 size = (u64)ddsImage->header.dwPitchOrLinearSize;
    u32 mipMapCount = (u32)ddsImage->header.dwMipMapCount;


    u64 offset = sizeof(DWORD) + sizeof(DDS_HEADER) + sizeof(DDS_HEADER_DXT10);
    u8* start = buffer+offset;
    
    u8* aFileBuffer = (u8*)malloc(sizeof(Image) + (fileSize-offset));
    Image* im = (Image*)aFileBuffer;
    im->magic[0] = 'A'; im->magic[1] = 'I'; im->magic[2] = 'M';  im->magic[3] = 'G';
    im->width = width;
    im->height = height;
    im->format = TextureFormat::COMPRESSED_RGBA_BC7;
    im->type = TextureType::ALBEDO;
    im->mipmapCount = mipMapCount;

    for (s32 i = 0; i < mipMapCount; ++i)
    {
        ImageMipmapHeader* mmHeader = (ImageMipmapHeader*)(aFileBuffer+(sizeof(Image) + (i * sizeof(ImageMipmapHeader))));
        u32 numBlocksWide = std::max<size_t>(1, (width + 3) / 4);
        u32 numBlocksHigh = std::max<size_t>(1, (height + 3) / 4);
        u32 rowBytes = numBlocksWide * 16;
        u32 numRows = numBlocksHigh;
        u32 numBytes = rowBytes * numRows;

        mmHeader->mipmapLevel = i;
        mmHeader->width = width;
        mmHeader->height = height;
        mmHeader->numOfBytes = numBytes;
        mmHeader->offset = sizeof(Image) + sizeof(ImageMipmapHeader)*mipMapCount + numBytes;
        void* imageData = (aFileBuffer+mmHeader->offset);
        memcpy(imageData, start, numBytes);

        start = start + numBytes;
        width /= 2;
        height /= 2;
    }

    fs::create_directories(gameFile.parent_path());
    FILE* aFile = fopen(gameFile.string().c_str(), "wb");
    size_t written = fwrite(aFileBuffer, 1, sizeof(Image) + (fileSize-offset), aFile);
    fclose(aFile);

    free(buffer);
    free(aFileBuffer);
}

void ConvertPNG2Image(fs::path gameFile, fs::path devFile) {
}

void ConvertFBX2Model(std::filesystem::path& gameFile, std::filesystem::path& devFile) {
    std::cout << "Implement FBX Loader" << std::endl;
}

void ConvertGLTF2Model(std::filesystem::path& gameFile, std::filesystem::path& devFile) {
    std::cout << "Implement gltf Loader" << std::endl;
}
