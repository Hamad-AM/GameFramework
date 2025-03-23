#include "asset_system.h"

#include <tiny_gltf.h>
//#include <encoder/basisu_comp.h>
//#include <transcoder/basisu_transcoder.h>
#include <stb_image.h>

//#define USE_KTX
//#define USE_GPU_COMPRESSION

#include <cstdlib>

#ifdef USE_KTX
    //#include <ktx.h>
#else
    //#include <DirectXTex.h>
#endif

#include <iostream>
#include <fstream>
#include <chrono>

#include <windows.h>

#define TimeNow() std::chrono::high_resolution_clock::now()
#define Timelapse(diff) std::chrono::duration_cast<std::chrono::microseconds>(end - start)

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

b32 LoadGLTFModel(std::string path, tinygltf::Model& model)
{
    std::filesystem::path filePath = path;
    tinygltf::TinyGLTF loader;
    //loader.SetImageLoader(nullptr, nullptr);
    std::string err;
    std::string warn;
    b32 ret;

    std::string ext = filePath.extension().string();
    std::string filenameWithoutExt = filePath.stem().string();
    filenameWithoutExt += "_no_images";
    std::filesystem::path noImageFilePath = filePath.parent_path() / (filenameWithoutExt + ".glb");
    b32 rewrite_file = false;
    if (!std::filesystem::exists(noImageFilePath))
    {
        rewrite_file = true;
    }
    else
    {
        std::filesystem::file_time_type noImageFileWriteTime = std::filesystem::last_write_time(noImageFilePath);
        std::filesystem::file_time_type withImageFileWriteTime = std::filesystem::last_write_time(path);

        // If image embeded Model is newer than the mesh only model
        auto timeSinceChange = std::chrono::duration_cast<std::chrono::seconds>(withImageFileWriteTime - noImageFileWriteTime);
        if (noImageFileWriteTime < withImageFileWriteTime)
        {
            rewrite_file = true;
        }
    }

    if (rewrite_file)
    {
        std::cout << filenameWithoutExt << " has been modified." << std::endl;
        tinygltf::Model tempModel;
        std::cout << "    Loading Model with Images..." << std::endl;
        ret = loader.LoadASCIIFromFile(&tempModel, &err, &warn, path);
        std::filesystem::path textureFilePath = filePath.parent_path() / "textures";
        std::string commandCall = "nvcompress -color -alpha -bc7 -fast -m " + textureFilePath.string() + " " + textureFilePath.string();
        s32 result = system(commandCall.c_str());
        for (tinygltf::Image& image : tempModel.images)
        {
            image.image.clear();
        }
        std::cout << "    Loaded Model With Images\n Cleared Images and writing mesh only model." << std::endl;
        loader.WriteGltfSceneToFile(&tempModel, noImageFilePath.string(), true, false, true, true);
    }
    std::cout << "Loading Binary Model... " << std::endl;
    ret = loader.LoadBinaryFromFile(&model, &err, &warn, noImageFilePath.string());
    std::cout << "Finished Loading" << std::endl;

    if (!ret)
    {
        std::cout << err << std::endl;
    }
    return ret;
}

// Function to convert a string to lowercase
std::string toLowercase(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

// Function to check if a string contains another string (case-insensitive)
bool containsSubstring(const std::string& mainStr, const std::string& subStr) {
    // Convert both strings to lowercase
    std::string lowerMainStr = toLowercase(mainStr);
    std::string lowerSubStr = toLowercase(subStr);

    // Check if the lowercase main string contains the lowercase substring
    return lowerMainStr.find(lowerSubStr) != std::string::npos;
}


void LoadImageFromFileAsync(AssetSystem& assets, Image& image)
{
    LoadImageJob job = {.path = image.filePath, .arena = &assets.arena, .name = image.name };

    std::lock_guard<std::mutex> lock(assets.queueMutex);

    assets.jobs.push(std::move(job));
    assets.condition.notify_one();
}

void AddImage(AssetSystem& assets, const tinygltf::Image& image, TextureType type, std::filesystem::path& filePath)
{
    const u8* imageData = image.image.data();
    std::string filename = image.name;
    if (assets.images.find(image.name) != assets.images.end()) return;
#ifdef USE_KTX
    filename += ".ktx2";
#else
    filename += ".dds";
#endif
    std::filesystem::path texture_file = filePath / filename;
    assets.images[image.name] = { .filePath = texture_file.string(), .name = image.name, .type = type };
}

b32 DoLoadImageJob(AssetSystem* assets, LoadImageJob& job)
{
    std::unique_lock<std::mutex> lock(assets->queueMutex);
    assets->condition.wait(lock, [assets] { return !assets->jobs.empty() || assets->stopLoading; });
    if (assets->stopLoading && assets->jobs.empty()) return false;
    job = std::move(assets->jobs.front());
    assets->jobs.pop();
    return true;
}


u8* AllocateAsset(AssetSystem* assets, u64& size)
{
    std::lock_guard<std::mutex> lock(assets->arenaMutex);
    u8* buffer = PushSize(&assets->arena, size);
    return buffer;
}

void LoadImageFromFile(AssetSystem* assets, std::string& path, std::string& name)
{
    FILE* file = fopen(path.c_str(), "rb");
    if (!file)
    {
        std::cerr << "Could not read file : " << path << std::endl;
    }

    fseek(file, 0, SEEK_END);
    u64 fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    u8* buffer = AllocateAsset(assets, fileSize);
    size_t bytesRead = fread(buffer, 1, fileSize, file);
    fclose(file);
    //std::cout << "Read file from : " << std::this_thread::get_id() << std::endl;

    DDSFile* ddsImage = (DDSFile*)buffer;

    u32 width = (u32)ddsImage->header.dwWidth;
    u32 height = (u32)ddsImage->header.dwHeight;
    u64 size = (u64)ddsImage->header.dwPitchOrLinearSize;
    u32 mipMapCount = (u32)ddsImage->header.dwMipMapCount;


    Image& im = assets->images[name];;
    im.width = width;
    im.height = height;
    im.format = texture_format::RGBA;
    u64 offset = sizeof(DWORD) + sizeof(DDS_HEADER) + sizeof(DDS_HEADER_DXT10);
    u8* start = buffer+offset;
    for (s32 i = 0; i < mipMapCount; ++i)
    {
        u32 numBlocksWide = std::max<size_t>(1, (width + 3) / 4);
        u32 numBlocksHigh = std::max<size_t>(1, (height + 3) / 4);
        u32 rowBytes = numBlocksWide * 16;
        u32 numRows = numBlocksHigh;
        u32 numBytes = rowBytes * numRows;

        u8* compressedImageData = buffer + offset;
        u64 imageSize = numBytes;
        im.imageData[i].width = width;
        im.imageData[i].height = height;
        im.imageData[i].data = start;
        im.imageData[i].bytes = numBytes;
        im.imageData[i].available = true;

        start = start + numBytes;
        width /= 2;
        height /= 2;
    }
}

void ShutDownLoading(AssetSystem& assets)
{
    std::lock_guard<std::mutex> lock(assets.queueMutex);
    assets.stopLoading = true;
    assets.condition.notify_all();
}

void LoadImageFromFile(AssetSystem& assets, const tinygltf::Image& image, std::filesystem::path& textures_path)
{
    if (assets.images.find(image.name) != assets.images.end())
    {
        return;
    }

    const u8* imageData = image.image.data();
    std::string filename = image.name;
#ifdef USE_KTX
    filename += ".ktx2";
#else
    filename += ".dds";
#endif
    std::filesystem::path texture_file = textures_path / filename;
    if (!std::filesystem::exists(texture_file))
    {
        std::cerr << "No compressed texture" << std::endl;
    }
#if 0
    u8* buffer = nullptr;
    size_t size;
    std::vector<u8> outImageData;
    s32 width, height, channels;
    std::filesystem::path pngTextureFile = texture_file.replace_extension(".png");
    u8* imageDatastb = stbi_load(pngTextureFile.string().c_str(), &width, &height, &channels, 4);
    buffer = PushSize(&assets.arena, width * height * 4 * sizeof(u8));
    memcpy(buffer, imageDatastb, width * height * 4 * sizeof(u8));
    size = width * height * 4 * sizeof(u8);
    stbi_image_free(imageDatastb);
#endif

    FILE* file = fopen(texture_file.string().c_str(), "rb");
    if (!file)
    {
        std::cerr << "Could not read file : " << filename << std::endl;
    }

    fseek(file, 0, SEEK_END);
    u64 fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    u8* buffer = PushSize(&assets.arena, fileSize);
    size_t bytesRead = fread(buffer, 1, fileSize, file);
    fclose(file);

    DDSFile* ddsImage = (DDSFile*)buffer;

    u32 width = (u32)ddsImage->header.dwWidth;
    u32 height = (u32)ddsImage->header.dwHeight;
    u64 size = (u64)ddsImage->header.dwPitchOrLinearSize;

    assets.images[image.name] = Image{};
    Image& i = assets.images[image.name];
    i.width = width;
    i.height = height;
    i.format = texture_format::RGBA;

    u32 numBlocksWide = std::max<size_t>(1, (width + 3) / 4);
    u32 numBlocksHigh = std::max<size_t>(1, (height + 3) / 4);
    u32 rowBytes = numBlocksWide * 16;
    u32 numRows = numBlocksHigh;
    u32 numBytes = rowBytes * numRows;

    u64 offset = sizeof(DWORD) + sizeof(DDS_HEADER) + sizeof(DDS_HEADER_DXT10);
    u8* compressedImageData = buffer + offset;
    u64 imageSize = numBytes;

}

Model GltfToModel(AssetSystem& assets, tinygltf::Model& model, std::string& path, std::unordered_map<std::string, u32> gpu_textures)
{
    Model result;
    for (const auto& glmesh : model.meshes) {
        for (const auto& primitive : glmesh.primitives) {
            Mesh mesh;

            bool calculateTangents = false;
            mesh.name = glmesh.name;
            if (mesh.name.find("decals") != std::string::npos) {
                continue;
            }

            if (primitive.attributes.find("NORMAL") == primitive.attributes.end()) {
                assert(("Mesh has no Normals", false));
            }

            if (primitive.attributes.find("TEXCOORD_0") == primitive.attributes.end()) {
                assert(("Mesh has no TexCoords", false));
            }

            if (primitive.attributes.find("TANGENT") == primitive.attributes.end()) {
                std::cout << "No Tangents found for : " << mesh.name << std::endl;
            }
         
            // Get vertex data
            const tinygltf::Accessor& positionAccessor = model.accessors[primitive.attributes.at("POSITION")];
            const tinygltf::BufferView& positionBufferView = model.bufferViews[positionAccessor.bufferView];
            const float* positions = reinterpret_cast<const float*>(&model.buffers[positionBufferView.buffer].data[positionBufferView.byteOffset + positionAccessor.byteOffset]);

            const auto& normalAccessor = model.accessors[primitive.attributes.at("NORMAL")];
            const auto& normalBufferView = model.bufferViews[normalAccessor.bufferView];
            const float* normals = reinterpret_cast<const float*>(&model.buffers[normalBufferView.buffer].data[normalBufferView.byteOffset + normalAccessor.byteOffset]);

            const auto& texCoordAccessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
            const auto& texCoordBufferView = model.bufferViews[texCoordAccessor.bufferView];
            const float* texCoordData = reinterpret_cast<const float*>(&model.buffers[texCoordBufferView.buffer].data[texCoordBufferView.byteOffset + texCoordAccessor.byteOffset]);

            const tinygltf::Accessor tangentAccessor = model.accessors[primitive.attributes.at("TANGENT")];
            const tinygltf::BufferView tangentBV = model.bufferViews[tangentAccessor.bufferView];
            const float* tangents = reinterpret_cast<const float*>(&model.buffers[tangentBV.buffer].data[tangentBV.byteOffset + tangentAccessor.byteOffset]);

            //if (positionAccessor.count == normalAccessor.count == texCoordAccessor.count == tangentAccessor.count))
            //{
            //    std::cout << "ERROR :: not equal count of vertex attributes " << mesh.name << std::endl;
            //}
    
            mesh.vertices.reserve(positionAccessor.count + normalAccessor.count + texCoordAccessor.count + tangentAccessor.count);
            for (size_t i = 0; i < positionAccessor.count; ++i)
            {
                mesh.vertices.insert(mesh.vertices.end(),
                        { positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2],
                          normals[i * 3 + 0], normals[i * 3 + 1], normals[i * 3 + 2],
                          texCoordData[i * 2 + 0], texCoordData[i * 2 + 1],
                          tangents[i * 3 + 0], tangents[i * 3 + 1], tangents[i * 3 + 2]
                        }
                );
            }

            // Get index data
            if (primitive.indices >= 0) {
                const auto& indexAccessor = model.accessors[primitive.indices];
                const auto& indexBufferView = model.bufferViews[indexAccessor.bufferView];
                if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                    const unsigned int* indices = reinterpret_cast<const unsigned int*>(&model.buffers[indexBufferView.buffer].data[indexBufferView.byteOffset + indexAccessor.byteOffset]);
                    for (size_t i = 0; i < indexAccessor.count; ++i)
                    {
                        mesh.indices.emplace_back(indices[i]);
                    }
                }
                else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                    const unsigned short* indices = reinterpret_cast<const unsigned short*>(&model.buffers[indexBufferView.buffer].data[indexBufferView.byteOffset + indexAccessor.byteOffset]);
                    for (size_t i = 0; i < indexAccessor.count; ++i)
                    {
                        mesh.indices.emplace_back(static_cast<unsigned int>(indices[i]));
                    }
                }
            }
            else {
                assert(("Mesh has no Normals", true));
            }

            if (calculateTangents)
            {
                for (u32 i = 0; i < mesh.indices.size() - 3; i += 3)
                {
                    u32 i0 = mesh.indices[i];
                    u32 i1 = mesh.indices[i + 1];
                    u32 i3 = mesh.indices[i + 2];
                }
            }
            std::filesystem::path textures_path = path;
            textures_path = textures_path.parent_path() / "textures";

            if (!std::filesystem::exists(textures_path))
            {
                if (std::filesystem::create_directories(textures_path))
                {
                    std::cout << "Create Folder here: " << textures_path.string() << std::endl;
                }
            }
            else
            {

            }

            s32 materialIndex = primitive.material;
            if (materialIndex >= 0 && materialIndex < model.materials.size())
            {
                const tinygltf::Material glMaterial = model.materials[materialIndex];
                // Access base color texture
                if (glMaterial.pbrMetallicRoughness.baseColorTexture.index >= 0) {
                    s32 textureIndex = glMaterial.pbrMetallicRoughness.baseColorTexture.index;
                    const tinygltf::Texture& texture = model.textures[textureIndex];
                    // Access the corresponding image
                    if (texture.source >= 0 && texture.source < model.images.size()) {
                        const tinygltf::Image& image = model.images[texture.source];
                        //std::shared_ptr<Image> myImage = std::make_shared<Image>(Image{ (u32)image.width, (u32)image.height, texture_format::RGBA, nullptr });
                        //images.emplace(textureIndex, myImage);
                        AddImage(assets, image, ALBEDO, textures_path);
                        mesh.albedo = image.name;
                    }
                }
                else
                {
                    std::cout << "Base Color map not found! " << glMaterial.name << std::endl;
                }

                
                // Access metallic roughness texture
                if (glMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0) {
                    s32 textureIndex = glMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;
                    const auto& texture = model.textures[textureIndex];

                    // Access the corresponding image
                    if (texture.source >= 0 && texture.source < model.images.size()) {
                        const auto& image = model.images[texture.source];
                        //std::shared_ptr<Image> myImage = std::make_shared<Image>(Image{ (u32)image.width, (u32)image.height, texture_format::RGBA, nullptr });
                        //myImage->image = (u8*)memcpy(sizeof(u32) * image.width * image.height);
                        //images.emplace(textureIndex, myImage);
                        AddImage(assets, image, METALLIC_ROUGHNESS, textures_path);
                        mesh.metallicRoughness = image.name;
                    }
                }
                else
                {
                    //TODO(): Put this back
                    //std::cout << "MetallicRoughness map not found! " << glMaterial.name << std::endl;
                }

                // Access normal texture if it exists
                if (glMaterial.normalTexture.index >= 0) {
                    s32 textureIndex = glMaterial.normalTexture.index;
                    const auto& texture = model.textures[textureIndex];

                    // Access the corresponding image
                    if (texture.source >= 0 && texture.source < model.images.size()) {
                        const auto& image = model.images[texture.source];
                        //std::shared_ptr<Image> myImage = std::make_shared<Image>(Image{ (u32)image.width, (u32)image.height, texture_format::RGBA, image.image });
                        //images.emplace(textureIndex, myImage);
                        AddImage(assets, image, NORMAL, textures_path);
                        mesh.normalMap = image.name;
                    }
                }
                else
                {
                    std::cout << "Normal map not found! " << glMaterial.name << std::endl;
                }
                std::string matName = glMaterial.name;
                if (glMaterial.name == "Plants_plants.DoubleSided")
                {
                    matName = "Plants_plants";
                }
                else if (glMaterial.name == "MASTER_Wood_Painted3.001")
                {
                    matName = "MASTER_Wood_Painted3";
                }
                auto it = std::find_if(model.images.begin(), model.images.end(), [&](const tinygltf::Image& image) {
                    return image.name.find(matName + "_Specular") != std::string::npos;
                });
                if (it != model.images.end())
                {
                    AddImage(assets, *it, SPECULAR, textures_path);
                    mesh.metallicRoughness = it->name;
                }
                else
                {
                    std::cout << "No Specular Map Found " << glMaterial.name << std::endl;
                }
            }

            result.meshes.push_back(mesh);
        }
    }

    return result;
}

void EncodeImageToKTX2(std::filesystem::path& filePath, const u8* imageData, u32 width, u32 height){
    //size_t fileSize = 0;
    //basisu::basisu_encoder_init(false, false);
    //void* data = basisu::basis_compress(basist::basis_tex_format::cUASTC4x4,
    //                                imageData,
    //                                width,
    //                                height,
    //                                0,
    //                                basisu::cFlagThreaded | basisu::cFlagPrintStats | basisu::cFlagDebug | basisu::cFlagPrintStatus,
    //                                0.5,
    //                                &fileSize,
    //                                nullptr);
    //filePath.replace_extension(".ktx2");
    //if (!basisu::write_data_to_file(filePath.string().c_str(), data, fileSize))
    //{
    //    std::cout << "Failed to write file : " << filePath << std::endl;
    //}
    //basisu::basis_free_data(data);
}

void TranscodeKTX2ToCompressedTexture(std::filesystem::path& filePath)
{
    //basist::basisu_transcoder_init();
    //basisu::uint8_vec ktx2_file_data;
    //filePath.replace_extension(".ktx2");
    //if (!basisu::read_file_to_vec(filePath.string().c_str(), ktx2_file_data))
    //    return;
    //basist::ktx2_transcoder transcoder;
    //if (!transcoder.init(ktx2_file_data.data(), ktx2_file_data.size_u32()))
    //{
    //    assert(false);
    //}
    //u32 width = transcoder.get_width();
    //u32 height = transcoder.get_height();
    //
    //transcoder.start_transcoding();
    //basisu::gpu_image tex(basisu::texture_format::cBC7, width, height);
    //
    //b32 status = transcoder.transcode_image_level(0, 0, 0, tex.get_ptr(), tex.get_total_blocks(), basist::transcoder_texture_format::cTFBC7_RGBA);
    //assert(status);
    //basisu::gpu_image_vec tex_vec;
    //tex_vec.push_back(tex);
    //filePath.replace_extension(".dds");
    //write_compressed_texture_file(filePath.string().c_str(), tex_vec, true);
}

void BatchModel(AssetSystem& assets, Model& model,
                std::vector<MeshRenderData>& batchMeshRenderData,
                std::unordered_map<std::string, u32> gpu_textures)
{
    auto& images = assets.images;

    std::chrono::microseconds totalMeshUploadTime = std::chrono::microseconds(0);
    std::chrono::microseconds totalTextureUploadTime = std::chrono::microseconds(0);;

    u32 vertexOffset = 0;
    u32 lastVertexCount = 0;
    u32 lastIndexCount = 0;
    auto start = TimeNow();
    for (auto image : assets.images)
    {
        GLuint textureID = CreateTexture(image.second);
        gpu_textures[image.first] = textureID;
    }
    auto end = TimeNow();
    auto duration = Timelapse(end - start);
    totalTextureUploadTime += duration;

    start = TimeNow();
    for (const Mesh& mesh : model.meshes)
    {
        batchMeshRenderData.push_back(MeshRenderData{});
        std::vector<MeshOffset> meshOffsets;
        MeshRenderData& meshRenderData = batchMeshRenderData.back();

        meshRenderData.numberOfIndices = mesh.indices.size();
        glGenBuffers(1, &meshRenderData.VBO);
        glGenBuffers(1, &meshRenderData.EBO);
        glGenVertexArrays(1, &meshRenderData.VAO);

        // Bind VAO
        glBindVertexArray(meshRenderData.VAO);

        // Upload vertex data
        glBindBuffer(GL_ARRAY_BUFFER, meshRenderData.VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(f32), &mesh.vertices[0], GL_STATIC_DRAW);

        // Upload index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshRenderData.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

        // Define vertex attribute pointers (assuming layout: position, normal, UV)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);       // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));  // Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));  // UV
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));  // Tangent
        glEnableVertexAttribArray(3);

        // Unbind VAO
        glBindVertexArray(0);


        if (gpu_textures.find(mesh.albedo) == gpu_textures.end())
        {
            std::cerr << "ERROR: can't find texture. Not uploaded to gpu" << std::endl;
        }
        else {
            meshRenderData.albedoID = gpu_textures[mesh.albedo];
        }

        if (gpu_textures.find(mesh.normalMap) == gpu_textures.end())
        {
            std::cerr << "ERROR: can't find texture. Not uploaded to gpu" << std::endl;
        }
        else {
            meshRenderData.normalMapID = gpu_textures[mesh.normalMap];
        }

        if (gpu_textures.find(mesh.metallicRoughness) == gpu_textures.end())
        {
            std::cerr << "ERROR: can't find texture. Not uploaded to gpu" << std::endl;
        }
        else {
            meshRenderData.metallicRoughnessID = gpu_textures[mesh.metallicRoughness];
        }
        // Record mesh offset
        //vertexOffset += lastrenderData.vertices.size();
        //lastVertexCount += renderData.vertices.size();
        //lastIndexCount += renderData.indices.size();
    }
    end = TimeNow();
    duration = Timelapse(end - start);
    totalMeshUploadTime += duration;

    std::cout << "  Time to Upload Mesh Data: " << totalMeshUploadTime.count() / 1000 << std::endl;
    std::cout << "  Time to Upload Texture Data: " << totalTextureUploadTime.count() / 1000 << std::endl;
}

const char* getGLErrorString(GLenum errorCode) {
    switch (errorCode) {
    case GL_NO_ERROR:              return "GL_NO_ERROR";
    case GL_INVALID_ENUM:          return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:         return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:     return "GL_INVALID_OPERATION";
    case GL_STACK_OVERFLOW:        return "GL_STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW:       return "GL_STACK_UNDERFLOW";
    case GL_OUT_OF_MEMORY:         return "GL_OUT_OF_MEMORY";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
    default:                       return "Unknown error";
    }
}

u32 CreateTexture(Image& image)
{

    GLuint textureID;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    switch(image.type)
    {
        // Normal maps should not have mipmap levels
    //case TextureType::NORMAL:
    //{
    //    ImageData& data = image.imageData[0];
    //    glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_BPTC_UNORM, data.width, data.height, 0, data.bytes, data.data);
    //    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //    break;
    //}
    default:
    {
        for (s32 i = 0; i < MAX_MIPMAP; ++i)
        {
            ImageData& data = image.imageData[i];
            glCompressedTexImage2D(GL_TEXTURE_2D, i, GL_COMPRESSED_RGBA_BPTC_UNORM, data.width, data.height, 0, data.bytes, data.data);
        }
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    }
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

void LookForWork(int threadId, AssetSystem* assets)
{
    while (true)
    {
        LoadImageJob job;
        if (!DoLoadImageJob(assets, job)) break;
        LoadImageFromFile(assets, job.path, job.name);
    }
}

void LoadScene(AssetSystem& assets, std::string path,
               std::vector<MeshRenderData>& batchMeshRenderData,
               std::unordered_map<std::string, u32> gpu_textures)
{
    Model model;
    tinygltf::Model glModel;
    auto start = TimeNow();
    b32 succesful = LoadGLTFModel(path, glModel);
    auto end = TimeNow();
    auto duration = Timelapse(end - start);
    std::cout << "Loading GLTF File with TinyGLTF " << duration.count() / 1000 << std::endl;

    start = TimeNow();
    if (succesful)
    {
        model = GltfToModel(assets, glModel, path, gpu_textures);
    }
    end = TimeNow();
    duration = Timelapse(end - start);
    std::cout << "Convert GLTF to Internal Model " << duration.count() / 1000 << std::endl;

    start = TimeNow();
    for(std::pair<const std::string, Image>& pair : assets.images)
    {
        LoadImageFromFileAsync(assets, pair.second); 
    }

    std::vector<std::thread> threads;
    for (s32 i = 0; i < 1; ++i)
    {
        threads.emplace_back(std::thread(LookForWork, i, &assets));
    }

    ShutDownLoading(assets);
    for (auto& worker : threads)
    {
        worker.join();
    }
    end = TimeNow();
    duration = Timelapse(end - start);
    std::cout << "Loading Image Files " << duration.count() / 1000 << std::endl;

    start = TimeNow();
    BatchModel(assets, model, batchMeshRenderData, gpu_textures);
    end = TimeNow();
    duration = Timelapse(end - start);
    std::cout << "Upload Mesh & Textures to GPU " << duration.count() / 1000 << std::endl;
    std::cout << "Finished Loading Scene" << std::endl;
}

void InitAssetSystem(AssetSystem& assets, MemoryArena arena)
{
    assets.arena = arena;
};
