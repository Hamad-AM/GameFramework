#include "../../src/common.h"
#include "AssetConverter.h"
#include <cstdio>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

#include <cstring>
#include <fstream>
#include <iostream>

static void writeSection(void* base, uint64_t destOffset, const void* src, size_t size) {
    memcpy((char*)base + destOffset, src, size);
}

// -------------------------------
// Helper to get raw pointer to GLTF buffer data
// -------------------------------
const uint8_t* GetGLTFBufferPointer(const tinygltf::Model& model, int accessorIndex, uint32_t* outStride) {
    if (accessorIndex < 0) return nullptr;
    const tinygltf::Accessor& acc = model.accessors[accessorIndex];
    const tinygltf::BufferView& bv = model.bufferViews[acc.bufferView];
    const tinygltf::Buffer& buf = model.buffers[bv.buffer];
    *outStride = (bv.byteStride == 0)
        ? tinygltf::GetComponentSizeInBytes(acc.componentType) * tinygltf::GetNumComponentsInType(acc.type)
        : bv.byteStride;
    return buf.data.data() + bv.byteOffset + acc.byteOffset;
}

// -------------------------------
// Validation & conversion
// -------------------------------

// Check that all required texture maps exist for every material.
// Also collect the actual texture indices used.
static void ValidateMaterials(const tinygltf::Model& model,
                              const std::filesystem::path& filePath) {
    std::cout << "Validating Materials\n";
    for (size_t matIdx = 0; matIdx < model.materials.size(); ++matIdx) {
        const auto& mat = model.materials[matIdx];
        bool hasAlbedo    = mat.pbrMetallicRoughness.baseColorTexture.index >= 0;
        bool hasNormal    = mat.normalTexture.index >= 0;
        bool hasMetalRough= mat.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0;

        if (!hasAlbedo || !hasNormal || !hasMetalRough) {
            std::cerr << "Error: Material " << mat.name << " (index " << matIdx << ") in file "
                      << filePath.string() << " is incomplete.\n"
                      << "  - albedo map: " << (hasAlbedo ? "yes" : "MISSING") << "\n"
                      << "  - normal map: " << (hasNormal ? "yes" : "MISSING") << "\n"
                      << "  - metallic-roughness map: " << (hasMetalRough ? "yes" : "MISSING") << "\n";
            assert(false && "Material missing required textures");
        }
    }
}

// Reject GPU compressed textures (DDS, KTX2). Only PNG/JPEG are allowed.
static void ValidateTextures(const tinygltf::Model& model,
                             const std::filesystem::path& filePath) {
    std::cout << "Validating Textures\n";
    for (size_t imgIdx = 0; imgIdx < model.images.size(); ++imgIdx) {
        const auto& img = model.images[imgIdx];
        std::string mime = img.mimeType;
        if (mime != "texture/jpeg" && mime != "image/png") {
            std::cerr << "Error: Texture " << img.name << " (index " << imgIdx << ") in file "
                      << filePath.string() << " has unsupported MIME type '" << mime << "'.\n"
                      << "Only PNG/JPEG are allowed.\n";
            assert(false && "Unsupported texture format");
        }
        // Optionally check channels (allow 3 or 4)
        if (img.component != 3 && img.component != 4) {
            std::cerr << "Error: Texture " << img.name << " has " << img.component
                      << " channels, expected 3 or 4.\n";
            assert(false && "Invalid channel count");
        }
    }
}

// Verify that every primitive has POSITION, NORMAL, TEXCOORD_0, TANGENT.
static void ValidateMeshes(const tinygltf::Model& model,
                           const std::filesystem::path& filePath) {
    std::cout << "Validating Mesh\n";
    for (size_t meshIdx = 0; meshIdx < model.meshes.size(); ++meshIdx) {
        const auto& mesh = model.meshes[meshIdx];
        for (size_t primIdx = 0; primIdx < mesh.primitives.size(); ++primIdx) {
            const auto& prim = mesh.primitives[primIdx];
            auto itPos = prim.attributes.find("POSITION");
            auto itNorm = prim.attributes.find("NORMAL");
            auto itUV = prim.attributes.find("TEXCOORD_0");
            auto itTan = prim.attributes.find("TANGENT");

            if (itPos == prim.attributes.end() ||
                itNorm == prim.attributes.end() ||
                itUV == prim.attributes.end() ||
                itTan == prim.attributes.end()) {
                std::cerr << "Error: Mesh '" << mesh.name << "' primitive " << primIdx
                          << " in file " << filePath.string() << " is missing required attributes.\n";
                if (itPos == prim.attributes.end()) std::cerr << "  - POSITION missing\n";
                if (itNorm == prim.attributes.end()) std::cerr << "  - NORMAL missing\n";
                if (itUV == prim.attributes.end()) std::cerr << "  - TEXCOORD_0 missing\n";
                if (itTan == prim.attributes.end()) std::cerr << "  - TANGENT missing\n";
                assert(false && "Primitive missing vertex attributes");
            }
        }
    }
}

// Convert RGB pixels to RGBA by adding 0xFF alpha.
static std::vector<uint8_t> ConvertRGBtoRGBA(const std::vector<uint8_t>& rgb, uint32_t width, uint32_t height) {
    std::vector<uint8_t> rgba(width * height * 4);
    for (size_t i = 0; i < width * height; ++i) {
        rgba[i*4+0] = rgb[i*3+0];
        rgba[i*4+1] = rgb[i*3+1];
        rgba[i*4+2] = rgb[i*3+2];
        rgba[i*4+3] = 0xFF;
    }
    return rgba;
}

// Main conversion routine.
void ConvertGLB(std::filesystem::path& gameFile, std::filesystem::path& file) {
    std::cout << "Converting : " << file.filename() << std::endl;
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string err, warn;

    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, file.string());
    if (!ret || !err.empty()) {
        std::cerr << "Failed to load GLB: " << err << "\n" << warn << "\n";
        assert(false && "GLB load failed");
    }

    std::cout << "Validating" << std::endl;
    // -------- Validation ----------
    ValidateTextures(model, file);
    ValidateMaterials(model, file);
    ValidateMeshes(model, file);
    std::cout << "Validation Successfull\n";

    // Count total primitives (each becomes a MeshHeader)
    uint32_t totalPrimitives = 0;
    for (const auto& m : model.meshes)
        totalPrimitives += static_cast<uint32_t>(m.primitives.size());

    // ---------- Build global buffers ----------
    uint64_t totalVertices = 0;
    uint64_t totalIndices = 0;

    // First pass: count vertices and indices
    for (const auto& gltfMesh : model.meshes) {
        for (const auto& prim : gltfMesh.primitives) {
            int posIdx = prim.attributes.at("POSITION");
            totalVertices += model.accessors[posIdx].count;
            totalIndices += model.accessors[prim.indices].count;
        }
    }

    // Allocate global buffers
    uint8_t* globalVertexBuffer = new uint8_t[totalVertices * sizeof(Vertex)];
    uint32_t* globalIndexBuffer = new uint32_t[totalIndices];
    MeshHeader* meshHeaders = new MeshHeader[totalPrimitives];


    // Second pass: copy vertex/index data and fill MeshHeaders
    uint64_t vertexCursor = 0;
    uint64_t indexCursor = 0;
    uint32_t meshIdx = 0;

    const char* meshName = "Mesh0";
    std::cout << "Allocating Mesh\n";
    for (const auto& gltfMesh : model.meshes) {
        for (const auto& prim : gltfMesh.primitives) {
            MeshHeader& mh = meshHeaders[meshIdx++];
            int posIdx = prim.attributes.at("POSITION");
            int normIdx = prim.attributes.at("NORMAL");
            int uvIdx = prim.attributes.at("TEXCOORD_0");
            int tanIdx = prim.attributes.at("TANGENT");
            int idxIdx = prim.indices;

            if (gltfMesh.name.size() > 256) {
                std::cerr << "Mesh name is too big" << std::endl;
            } else {
                std::copy_n(gltfMesh.name.begin(), gltfMesh.name.size(), mh.name);
                mh.name[gltfMesh.name.size()] = '\0';
            }

            mh.vertexCount = model.accessors[posIdx].count;
            mh.indexCount  = model.accessors[idxIdx].count;
            mh.vertexOffset = static_cast<uint32_t>(vertexCursor * sizeof(Vertex));
            mh.indexOffset  = static_cast<uint32_t>(indexCursor * sizeof(uint32_t));
            mh.materialIdx = (prim.material >= 0) ? static_cast<uint32_t>(prim.material) : 0; // fallback to first material

            // Get raw pointers & strides
            uint32_t posStride, normStride, uvStride, tanStride, idxStride;
            const uint8_t* pPos  = GetGLTFBufferPointer(model, posIdx, &posStride);
            const uint8_t* pNorm = GetGLTFBufferPointer(model, normIdx, &normStride);
            const uint8_t* pUV   = GetGLTFBufferPointer(model, uvIdx, &uvStride);
            const uint8_t* pTan  = GetGLTFBufferPointer(model, tanIdx, &tanStride);
            const uint8_t* pIdx  = GetGLTFBufferPointer(model, idxIdx, &idxStride);

            // Copy vertices (interleaved)
            Vertex* destVertices = reinterpret_cast<Vertex*>(globalVertexBuffer + mh.vertexOffset);
            for (uint64_t i = 0; i < mh.vertexCount; ++i) {
                Vertex& v = destVertices[i];
                memcpy(v.position, pPos + i * posStride, 3 * sizeof(float));
                memcpy(v.normal,  pNorm + i * normStride, 3 * sizeof(float));
                memcpy(v.uv,      pUV   + i * uvStride,   2 * sizeof(float));
                memcpy(v.tangent, pTan  + i * tanStride,  3 * sizeof(float));
            }

            // Copy indices (convert to uint32_t)
            uint32_t* destIndices = globalIndexBuffer + indexCursor;
            const tinygltf::Accessor& idxAcc = model.accessors[idxIdx];
            for (uint64_t i = 0; i < mh.indexCount; ++i) {
                if (idxAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT) {
                    destIndices[i] = *reinterpret_cast<const uint16_t*>(pIdx + i * idxStride);
                } else if (idxAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT) {
                    destIndices[i] = *reinterpret_cast<const uint32_t*>(pIdx + i * idxStride);
                } else {
                    assert(false && "Unsupported index component type");
                }
            }

            vertexCursor += mh.vertexCount;
            indexCursor  += mh.indexCount;
        }
    }
    std::cout << "Finished allocating mesh\n";

    std::cout << "Allocating Textures\n";
    // ---------- Build texture headers and prepare pixel data ----------
    uint32_t textureCount = static_cast<uint32_t>(model.images.size());
    TextureHeader* textureHeaders = new TextureHeader[textureCount];
    std::vector<std::vector<uint8_t>> texturePixelData(textureCount); // store converted RGBA data

    for (uint32_t i = 0; i < textureCount; ++i) {
        const tinygltf::Image& img = model.images[i];
        TextureHeader& hdr = textureHeaders[i];
        hdr.width  = static_cast<uint32_t>(img.width);
        hdr.height = static_cast<uint32_t>(img.height);
        hdr.channels = (img.component == 3) ? 3 : 4; // keep original, but we'll store as RGBA for simplicity
        hdr.format = (hdr.channels == 4) ? TextureFormat::RGBA8 : TextureFormat::RGB8;
        hdr.type = TextureType::ALBEDO;
        hdr.mipmapCount = 1;   // no mipmaps for now

        if (img.name.size() > 256) {
            std::cerr << "Image name is too big" << std::endl;
        } else {
            std::copy_n(img.name.begin(), img.name.size(), hdr.name);
            hdr.name[img.name.size()] = '\0';
        }

        // Convert to RGBA if needed (makes rendering easier)
        if (img.component == 3) {
            texturePixelData[i] = ConvertRGBtoRGBA(img.image, img.width, img.height);
            hdr.channels = 4;
            hdr.format = TextureFormat::RGBA8;
        } else if (img.component == 4) {
            texturePixelData[i].assign(img.image.begin(), img.image.end());
        } else {
            assert(false && "Unexpected channel count");
        }
    }
    std::cout << "Finished Allocating Textures\n";

    std::cout << "Allocating Materials\n";
    // For materials: we will keep all original materials (may be referenced by primitives)
    uint32_t materialCount = static_cast<uint32_t>(model.materials.size());
    MaterialHeader* materialHeaders = new MaterialHeader[materialCount];

    // Fill material headers: map glTF texture indices to our texture array indices.
    // Our texture array will contain all textures in the same order as model.images.
    for (uint32_t matIdx = 0; matIdx < materialCount; ++matIdx) {
        const auto& gltfMat = model.materials[matIdx];
        materialHeaders[matIdx].albedoIdx = static_cast<uint32_t>(model.textures[gltfMat.pbrMetallicRoughness.baseColorTexture.index].source);
        materialHeaders[matIdx].normalIdx = static_cast<uint32_t>(model.textures[gltfMat.normalTexture.index].source);
        materialHeaders[matIdx].metallicRoughnessIdx = static_cast<uint32_t>(model.textures[gltfMat.pbrMetallicRoughness.metallicRoughnessTexture.index].source);
        TextureHeader& albedo = textureHeaders[materialHeaders[matIdx].albedoIdx];
        albedo.type = TextureType::ALBEDO;
        TextureHeader& normal = textureHeaders[materialHeaders[matIdx].normalIdx];
        normal.type = TextureType::NORMAL;
        TextureHeader& roughMetal = textureHeaders[materialHeaders[matIdx].metallicRoughnessIdx];
        roughMetal.type = TextureType::METALLIC_ROUGHNESS;
    }
    std::cout << "Finished\n";

    std::cout << "Writing to file\n";

// ---------- 3. Compute total file size and offsets ----------
    uint64_t fileSize = 0;
    SceneHeader ls; // temporary to compute offsets
    ls.magic[0] = 'A'; ls.magic[1] = 'S'; ls.magic[2] = 'C'; ls.magic[3] = 'N';

    if (file.filename().string().size() > 256) {
        std::cerr << "Image name is too big" << std::endl;
    } else {
        std::copy_n(file.filename().string().begin(), file.filename().string().size(), ls.name);
        ls.name[file.filename().string().size()] = '\0';
    }

    ls.meshCount = totalPrimitives;
    ls.textureCount = (uint32_t)model.images.size();
    ls.materialCount = (uint32_t)model.materials.size();

    fileSize += sizeof(SceneHeader);
    ls.meshHeadersOffset = (uint32_t)fileSize;
    fileSize += totalPrimitives * sizeof(MeshHeader);
    ls.materialHeadersOffset = (uint32_t)fileSize;
    fileSize += model.materials.size() * sizeof(MaterialHeader);
    ls.textureHeadersOffset = (uint32_t)fileSize;
    fileSize += model.images.size() * sizeof(TextureHeader);
    ls.vertexBufferOffset = fileSize;
    fileSize += totalVertices * sizeof(Vertex);
    ls.indexBufferOffset = fileSize;
    fileSize += totalIndices * sizeof(uint32_t);
    ls.textureDataOffset = fileSize;
    // compute total texture pixel size
    uint64_t totalTexSize = 0;
    for (const auto& pixels : texturePixelData) totalTexSize += pixels.size();
    fileSize += totalTexSize;
    // optional: store per-texture offsets (if you want random access without summing sizes)
    ls.textureOffsetsOffset = fileSize;
    fileSize += model.images.size() * sizeof(uint64_t);

    // ---------- 4. Allocate the big block ----------
    uint8_t* fileData = new uint8_t[fileSize];
    // Write LoadedScene at offset 0
    memcpy(fileData, &ls, sizeof(SceneHeader));

    // Write MeshHeaders
    writeSection(fileData, ls.meshHeadersOffset, meshHeaders, totalPrimitives * sizeof(MeshHeader));
    // Write MaterialHeaders
    writeSection(fileData, ls.materialHeadersOffset, materialHeaders, model.materials.size() * sizeof(MaterialHeader));
    // Write textureHeaders
    writeSection(fileData, ls.textureHeadersOffset, textureHeaders, model.images.size() * sizeof(TextureHeader));
    // Write Vertex buffer
    writeSection(fileData, ls.vertexBufferOffset, globalVertexBuffer, totalVertices * sizeof(Vertex));
    // Write Index buffer
    writeSection(fileData, ls.indexBufferOffset, globalIndexBuffer, totalIndices * sizeof(uint32_t));
    // Write Texture pixel data (concatenated)
    uint64_t texWriteOffset = ls.textureDataOffset;
    for (const auto& pixels : texturePixelData) {
        writeSection(fileData, texWriteOffset, pixels.data(), pixels.size());
        texWriteOffset += pixels.size();
    }
    // Write per‑texture offsets (optional)
    uint64_t* texOffs = (uint64_t*)(fileData + ls.textureOffsetsOffset);
    uint64_t runningOffset = 0;
    for (uint32_t i = 0; i < model.images.size(); ++i) {
        texOffs[i] = runningOffset;
        runningOffset += texturePixelData[i].size();
    }

    // ---------- 5. Write the whole block to disk ----------
    FILE* f = fopen(gameFile.string().c_str(), "wb");
    std::cout << gameFile.string() << "\n";
    assert(f);
    fwrite(fileData, 1, fileSize, f);
    fclose(f);
    std::cout << "finished writing file\n";

    // Cleanup temporary arrays (meshHeaders, materialHeaders, textureHeaders, globalVertexBuffer, globalIndexBuffer, imagePixelData)
    delete[] fileData;    // Cleanup
    delete[] globalVertexBuffer;
    delete[] globalIndexBuffer;
    delete[] meshHeaders;
    delete[] materialHeaders;
    delete[] textureHeaders;

    std::cout << "Successfully converted " << file.string() << " -> " << gameFile.string() << "\n";
}

#if 0
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
#endif
