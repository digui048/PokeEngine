#include "MeshImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "Poke/Core/Log.h"

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

std::shared_ptr<Mesh> MeshImporter::LoadMesh(const std::string &filepath)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        POKE_CORE_ERROR("Assimp filed: {0}", std::string(importer.GetErrorString()));
    }

    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    std::unordered_map<Vertex, uint16_t> uniqueVertices{};

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        aiMesh *mesh = scene->mMeshes[i];
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
    }

    return std::make_shared<Mesh>(vertices, indices);
}