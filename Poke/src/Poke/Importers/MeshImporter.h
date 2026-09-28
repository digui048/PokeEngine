#ifndef MESH_IMPORTER_H
#define MESH_IMPORTER_H

#include "Poke/Resources/Assets/Mesh.h"
#include <string>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Poke
{
    class GameObject;
    class MeshImporter
    {
    public:
        MeshImporter() = delete;

        static std::shared_ptr<Mesh> LoadMesh(aiMesh *mesh, AssetHandle handle);
    };
}

#endif