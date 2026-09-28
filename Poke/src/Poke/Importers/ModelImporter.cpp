#include "ModelImporter.h"

#include "Poke/Importers/MeshImporter.h"
#include "Poke/Core/Log.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

using namespace Poke;

AssetImportResult ModelImporter::LoadModel(const AssetMetaData &metadata)
{
    AssetImportResult result;
    Assimp::Importer importer;
    importer.SetPropertyInteger(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);
    const aiScene *scene = importer.ReadFile(metadata.filePath.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        POKE_CORE_ERROR("[ModelImporter] Assimp filed: {0}", std::string(importer.GetErrorString()));
        return result;
    }

    std::vector<AssetHandle> meshHandles(scene->mNumMeshes);
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        AssetHandle meshHandle;
        std::shared_ptr<Mesh> mesh = MeshImporter::LoadMesh(scene->mMeshes[i], meshHandle);

        if (!mesh)
        {
            POKE_CORE_ERROR("[ModelImporter] Failed to import mesh {0}", i);

            return AssetImportResult{};
        }

        meshHandles[i] = meshHandle;
        result.subAssets.push_back(mesh);
    }

    Model::Node rootNode = ProcessNode(scene->mRootNode, scene, meshHandles);
    result.asset = std::make_shared<Model>(metadata.handle, std::move(rootNode));
    
    return result;
}

Model::Node ModelImporter::ProcessNode(aiNode *node, const aiScene *scene, const std::vector<AssetHandle> &meshHandles)
{
    Model::Node modelNode;
    modelNode.name = node->mName.C_Str();
    modelNode.localTransform = ConvertMatrix(node->mTransformation);

    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        unsigned int meshIndex = node->mMeshes[i];
        modelNode.meshes.push_back(meshHandles[meshIndex]);
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        modelNode.children.push_back(ProcessNode(node->mChildren[i], scene, meshHandles));
    }

    return modelNode;
}

glm::mat4 ModelImporter::ConvertMatrix(const aiMatrix4x4 &aiMat)
{
    glm::mat4 matrix;
    matrix[0][0] = aiMat.a1;
    matrix[0][1] = aiMat.b1;
    matrix[0][2] = aiMat.c1;
    matrix[0][3] = aiMat.d1;

    matrix[1][0] = aiMat.a2;
    matrix[1][1] = aiMat.b2;
    matrix[1][2] = aiMat.c2;
    matrix[1][3] = aiMat.d2;

    matrix[2][0] = aiMat.a3;
    matrix[2][1] = aiMat.b3;
    matrix[2][2] = aiMat.c3;
    matrix[2][3] = aiMat.d3;

    matrix[3][0] = aiMat.a4;
    matrix[3][1] = aiMat.b4;
    matrix[3][2] = aiMat.c4;
    matrix[3][3] = aiMat.d4;

    return matrix;
}
