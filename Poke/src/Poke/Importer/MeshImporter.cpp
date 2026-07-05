#include "MeshImporter.h"

#include "Poke/Core/Log.h"
#include "Poke/Resources/Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace Poke;

std::vector<Mesh> MeshImporter::Import(const std::string &filepath)
{
    std::vector<Mesh> importedMesh;

    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        POKE_CORE_ERROR("Assimp failed to load file: {0}", importer.GetErrorString());
        return importedMesh;
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        aiMesh *mesh = scene->mMeshes[i];

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int j = 0; j < mesh->mNumVertices; ++j)
        {
            Vertex vertex;

            vertex.Position = {mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z};

            if (mesh->HasNormals())
            {
                vertex.Normal = {mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z};
            }
            else
            {
                vertex.Normal = {0.0f, 0.0f, 0.0f};
            }

            if (mesh->mTextureCoords[0])
            {
                vertex.TexCoords = {mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y};
            }
            else
            {
                vertex.TexCoords = {0.0f, 0.0f};
            }

            vertices.push_back(vertex);
        }

        for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
        {
            aiFace face = mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; ++k)
            {
                indices.push_back(face.mIndices[k]);
            }
        }

        importedMesh.emplace_back(vertices, indices);
    }

    return importedMesh;
}