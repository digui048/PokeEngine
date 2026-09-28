#include "MeshImporter.h"
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

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

std::shared_ptr<Mesh> MeshImporter::LoadMesh(aiMesh *mesh, AssetHandle handle)
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

    return std::make_shared<Mesh>(handle, vertices, indices);
}