#ifndef MESH_IMPORTER_H
#define MESH_IMPORTER_H

#include <string>
#include <vector>

namespace Poke
{
    class Mesh;

    class MeshImporter
    {
    public:
        static std::vector<Mesh> Import(const std::string &filepath);
    };
}

#endif