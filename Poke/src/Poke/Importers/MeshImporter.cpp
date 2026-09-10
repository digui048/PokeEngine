#include "MeshImporter.h"
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "Poke/Core/Log.h"
#include "Poke/Scene/GameObject.h"
#include "Poke/Scene/Components/MeshComponent.h"
#include "Poke/Scene/Components/TransformComponent.h"

namespace std
{
    template <>
    struct hash<Poke::Vertex>
    {
        size_t operator()(Poke::Vertex const &vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1 ^ (hash<glm::vec2>()(vertex.texCoord) << 1));
        }
    };
}

using namespace Poke;

void MeshImporter::LoadHierarchy(const std::string &filepath, GameObject *rootObject)
{
    Assimp::Importer importer;
    importer.SetPropertyInteger(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);
    const aiScene *scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        POKE_CORE_ERROR("Assimp filed: {0}", std::string(importer.GetErrorString()));
        return;
    }

    ProcessNode(scene->mRootNode, scene, rootObject);
}

void MeshImporter::ProcessNode(aiNode *node, const aiScene *scene, GameObject *parentObject)
{
    GameObject *currentObject = parentObject;

    if (node != scene->mRootNode)
    {
        auto child = std::make_unique<GameObject>(node->mName.C_Str());
        currentObject = child.get();

        aiMatrix4x4 aiMat = node->mTransformation;
        glm::mat4 localMat;
        localMat[0][0] = aiMat.a1;
        localMat[0][1] = aiMat.b1;
        localMat[0][2] = aiMat.c1;
        localMat[0][3] = aiMat.d1;
        localMat[1][0] = aiMat.a2;
        localMat[1][1] = aiMat.b2;
        localMat[1][2] = aiMat.c2;
        localMat[1][3] = aiMat.d2;
        localMat[2][0] = aiMat.a3;
        localMat[2][1] = aiMat.b3;
        localMat[2][2] = aiMat.c3;
        localMat[2][3] = aiMat.d3;
        localMat[3][0] = aiMat.a4;
        localMat[3][1] = aiMat.b4;
        localMat[3][2] = aiMat.c4;
        localMat[3][3] = aiMat.d4;

        currentObject->GetTransform()->SetLocalTransform(localMat);

        parentObject->AddChild(std::move(child));
    }

    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        std::shared_ptr<Mesh> parsedMesh = ProcessMesh(mesh, scene);
        currentObject->AddComponent<MeshComponent>(parsedMesh);
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(node->mChildren[i], scene, currentObject);
    }
}

std::shared_ptr<Mesh> MeshImporter::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    std::unordered_map<Vertex, uint16_t> uniqueVertices{};

    for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
    {
        aiFace face = mesh->mFaces[j];
        for (unsigned int k = 0; k < face.mNumIndices; ++k)
        {
            unsigned int index = face.mIndices[k];
            Vertex vertex{};

            vertex.pos = {mesh->mVertices[index].x, mesh->mVertices[index].y, mesh->mVertices[index].z};
            vertex.color = {1.0f, 1.0f, 1.0f};

            if (mesh->mTextureCoords[0])
            {
                vertex.texCoord = {mesh->mTextureCoords[0][index].x, mesh->mTextureCoords[0][index].y};
            }
            else
            {
                vertex.texCoord = {0.0f, 0.0f};
            }

            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint16_t>(vertices.size());
                vertices.push_back(vertex);
            }
            indices.push_back(uniqueVertices[vertex]);
        }
    }

    return std::make_shared<Mesh>(vertices, indices);
}