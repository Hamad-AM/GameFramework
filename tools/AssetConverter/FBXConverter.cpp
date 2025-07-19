#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

void ConvertFBX2Model(fs::path& gameFile, fs::path& file) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file, aiProcess_Traingulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "Error Assimp: " << importer.GetErrorString() << std::endl;
        return;
    }
    std::vector<aiNode*> nodes = {scene->mRootNode};
    std::vector<aiMesh*> meshes;
    while (nodes.size() > 0) {
        aiNode* node = nodes[0];
        for (u32 i = 0; i < node->mNumMeshes; ++i) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(mesh);
        }
        for (u32 i = 0; i < node->mNumChildren; ++i) {
            nodes.push_back(node->mChildren[i]);
        }
        nodes.erase(nodes.begin());
    }
}
