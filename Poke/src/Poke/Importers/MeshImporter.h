#ifndef MESH_IMPORTER_H
#define MESH_IMPORTER_H

#include "Poke/Resources/Mesh.h"
#include <string>
#include <memory>

namespace Poke
{
    class MeshImporter
    {
    public:
        MeshImporter() = delete;

        static std::shared_ptr<Mesh> LoadMesh(const std::string &filepath);
    };
}

#endif