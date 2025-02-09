#include "asset_system.h"

#include <tiny_gltf.h>
#include <encoder/basisu_comp.h>
#include <transcoder/basisu_transcoder.h>

#include <iostream>

b32 LoadGLTFModel(std::string path, tinygltf::Model& model)
{
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;
    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, path);
    if (!ret)
    {
        std::cout << err << std::endl;
    }
    return ret;
}

Model GltfToModel(AssetSystem& assets, tinygltf::Model& model, std::string& path, std::unordered_map<s32, u32> gpu_textures)
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

            // Get vertex data
            const tinygltf::Accessor& positionAccessor = model.accessors[primitive.attributes.at("POSITION")];
            const tinygltf::BufferView& positionBufferView = model.bufferViews[positionAccessor.bufferView];
            const float* positions = reinterpret_cast<const float*>(&model.buffers[positionBufferView.buffer].data[positionBufferView.byteOffset + positionAccessor.byteOffset]);

            for (size_t i = 0; i < positionAccessor.count; ++i)
            {
                mesh.positions.emplace_back(glm::vec3(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]));
            }

            // Get normal data if available
            if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                const auto& normalAccessor = model.accessors[primitive.attributes.at("NORMAL")];
                const auto& normalBufferView = model.bufferViews[normalAccessor.bufferView];
                const float* normals = reinterpret_cast<const float*>(&model.buffers[normalBufferView.buffer].data[normalBufferView.byteOffset + normalAccessor.byteOffset]);
                for (size_t i = 0; i < normalAccessor.count; ++i)
                {
                    mesh.normals.emplace_back(glm::vec3(normals[i * 3 + 0], normals[i * 3 + 1], normals[i * 3 + 2]));
                }
            }
            else {
                assert(("Mesh has no Normals", false));
            }

            if (primitive.attributes.count("TEXCOORD_0")) {
                const auto& texCoordAccessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
                const auto& texCoordBufferView = model.bufferViews[texCoordAccessor.bufferView];
                const float* texCoordData = reinterpret_cast<const float*>(&model.buffers[texCoordBufferView.buffer].data[texCoordBufferView.byteOffset + texCoordAccessor.byteOffset]);
                for (size_t i = 0; i < texCoordAccessor.count; ++i)
                {
                    mesh.uvs.emplace_back(glm::vec2(texCoordData[i * 2 + 0], texCoordData[i * 2 + 1]));
                }
            }
            else {
                assert(("Mesh has no TexCoords", false));
            }

            // if (primitive.attributes.find("TANGENT") != primitive.attributes.end()) {
            //     const tinygltf::Accessor tangentAccessor = model.accessors[primitive.attributes.at("TANGENT")];
            //     const tinygltf::BufferView tangentBV = model.bufferViews[tangentAccessor.bufferView];
            //     const float* tangents = reinterpret_cast<const float*>(&model.buffers[tangentBV.buffer].data[tangentBV.byteOffset + tangentAccessor.byteOffset]);
            //     const glm::vec3& normal = {[mesh.normals.back();
            //     for (size_t i = 0; i < tangentAccessor.count; ++i)
            //     {
            //         mesh.tangents.emplace_back(glm::vec3(tangents[i * 3 + 0], tangents[i * 3 + 1], tangents[i * 3 + 2]));
            //         const glm::vec3& tangent = mesh.tangents.back();
            //         mesh.bitangents.emplace_back(glm::cross(tangent, normal));
            //     }
            // }
            // else {
            //     // calculate tangents as they are not available
            //     calculateTangents = true;
            // }

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
                        const u8* imageData = image.image.data();
                        std::filesystem::path texture_file = textures_path / image.name;
                        EncodeImageToKTF2(texture_file, imageData, image.width, image.height);
                        TranscodeKTX2ToCompressedTexture(texture_file);
                        mesh.albedo = textureIndex;
                    }
                    std::string name = texture.name;
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
                        mesh.metallicRoughness = textureIndex;
                    }
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
                        mesh.normalMap = textureIndex;
                    }
                }
            }
            result.meshes.push_back(mesh);
        }
    }
    return result;
}

void EncodeImageToKTX2(std::filesystem::path& filePath, const u8* imageData, u32 width, u32 height){
    size_t fileSize = 0;
    void* data = basisu::basis_compress(basist::basis_tex_format::cUASTC4x4,
                                    imageData,
                                    width,
                                    height,
                                    0,
                                    basisu::cFlagThreaded | basisu::cFlagPrintStats | basisu::cFlagDebug | basisu::cFlagPrintStatus,
                                    1,
                                    &fileSize,
                                    nullptr);
    std::filesystem::path texture_file = filePath / ".ktx2";
    if (!basisu::write_data_to_file(texture_file.string().c_str(), data, fileSize))
    {
        std::cout << "Failed to write file : " << texture_file << std::endl;
    }
}

void TranscodeKTX2ToCompressedTexture(std::filesystem::path& filePath)
{
    basist::basisu_transcoder_init();
    basisu::uint8_vec ktx2_file_data;
}

void BatchModel(AssetSystem& assets, Model& model,
                std::vector<MeshRenderData>& batchMeshRenderData,
                std::unordered_map<s32, u32> gpu_textures)
{
    auto& images = assets.images;

    u32 vertexOffset = 0;
    u32 lastVertexCount = 0;
    u32 lastIndexCount = 0;
    for (const Mesh& mesh : model.meshes)
    {
        batchMeshRenderData.push_back(MeshRenderData{});
        std::vector<MeshOffset> meshOffsets;
        std::vector<f32> vertices;
        std::vector<u32> indices;
        MeshRenderData& meshRenderData = batchMeshRenderData.back();
        for (size_t i = 0; i < mesh.positions.size(); ++i)
        {
            vertices.push_back(mesh.positions[i].x);
            vertices.push_back(mesh.positions[i].y);
            vertices.push_back(mesh.positions[i].z);
            vertices.push_back(mesh.normals[i].x);
            vertices.push_back(mesh.normals[i].y);
            vertices.push_back(mesh.normals[i].z);
            vertices.push_back(mesh.uvs[i].x);
            vertices.push_back(mesh.uvs[i].y);
        }

        // Add indices with offset
        for (const u32& index : mesh.indices) {
            indices.push_back(index + vertexOffset / 8);
        }

        meshRenderData.numberOfIndices = indices.size();
        glGenBuffers(1, &meshRenderData.VBO);
        glGenBuffers(1, &meshRenderData.EBO);
        glGenVertexArrays(1, &meshRenderData.VAO);

        // Bind VAO
        glBindVertexArray(meshRenderData.VAO);

        // Upload vertex data
        glBindBuffer(GL_ARRAY_BUFFER, meshRenderData.VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(f32), &vertices[0], GL_STATIC_DRAW);

        // Upload index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshRenderData.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Define vertex attribute pointers (assuming layout: position, normal, UV)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);       // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));  // Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));  // UV
        glEnableVertexAttribArray(2);

        // Unbind VAO
        glBindVertexArray(0);

        if (gpu_textures.find(mesh.albedo) == gpu_textures.end())
        {
            Image* image = images[mesh.albedo];

            GLuint textureID;

            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, &image->data[0]);
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);

            gpu_textures[mesh.albedo] = textureID;
            meshRenderData.albedoID = textureID;
        }
        else {
            meshRenderData.albedoID = gpu_textures[mesh.albedo];
        }

        if (gpu_textures.find(mesh.normalMap) == gpu_textures.end())
        {
            Image* image = images[mesh.normalMap];

            GLuint textureID;

            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, &image->data[0]);
            glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);

            gpu_textures[mesh.normalMap] = textureID;
            meshRenderData.normalMapID = textureID;
        }
        else {
            meshRenderData.normalMapID = gpu_textures[mesh.normalMap];
        }

        if (gpu_textures.find(mesh.metallicRoughness) == gpu_textures.end())
        {
            Image* image = images[mesh.metallicRoughness];

            GLuint textureID;

            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, &image->data[0]);
            glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);

            gpu_textures[mesh.metallicRoughness] = textureID;
            meshRenderData.metallicRoughnessID = textureID;
        }
        else {
            meshRenderData.metallicRoughnessID = gpu_textures[mesh.metallicRoughness];
        }

        // Record mesh offset
        //vertexOffset += lastrenderData.vertices.size();
        //lastVertexCount += renderData.vertices.size();
        //lastIndexCount += renderData.indices.size();
    }
}

void LoadScene(AssetSystem& assets, std::string path,
               std::vector<MeshRenderData>& batchMeshRenderData,
               std::unordered_map<s32, u32> gpu_textures)
{
    Model model;
    tinygltf::Model glModel;
    b32 succesful = LoadGLTFModel(path, glModel);
    if (succesful)
    {
        model = GltfToModel(assets, glModel, path, gpu_textures);
    }
    BatchModel(assets, model, batchMeshRenderData, gpu_textures);
}


void InitAssetSystem(AssetSystem& assets)
{
    return;
};
