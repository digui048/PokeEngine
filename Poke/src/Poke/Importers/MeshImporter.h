#ifndef MESH_IMPORTER_H
#define MESH_IMPORTER_H

#include "Poke/Resources/Mesh.h"
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

        static void LoadHierarchy(const std::string &filepath, GameObject *rootObject);
        static void ProcessNode(aiNode *node, const aiScene *scene, GameObject *parentObject);
        static std::shared_ptr<Mesh> ProcessMesh(aiMesh *mesh, const aiScene *scene);
    };
}

#endif