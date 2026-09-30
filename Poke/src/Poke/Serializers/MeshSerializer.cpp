#include "MeshSerializer.h"

#include "Poke/Resources/Assets/Mesh.h"
#include "Poke/Core/Log.h"

#include <fstream>

using namespace Poke;

bool MeshSerializer::SerializeToLibrary(const AssetMetaData &metadata, const std::shared_ptr<Asset> &asset, const std::filesystem::path &binaryPath) const
{
    if (!asset)
    {
        POKE_CORE_ERROR("[MeshSerializer] Can not serialize null asset for handle {0}", static_cast<uint64_t>(metadata.handle));
        return false;
    }

    auto mesh = std::static_pointer_cast<Mesh>(asset);

    MeshHeader header;
    header.vertexCount = static_cast<uint32_t>(mesh->GetVerticesCount());
    header.indexCount = static_cast<uint32_t>(mesh->GetIndicesCount());

    std::filesystem::create_directories(binaryPath.parent_path());

    std::ofstream file(binaryPath, std::ios::binary);
    if (!file.is_open())
    {
        POKE_CORE_ERROR("[MeshSerializer] Failed to open: {0}", binaryPath.string());
        return false;
    }

    file.write(reinterpret_cast<const char *>(&header), sizeof(header));
    for (const Vertex &vertex : mesh->GetVertices())
    {
        file.write(reinterpret_cast<const char *>(&vertex.pos.x), sizeof(float) * 3);
        file.write(reinterpret_cast<const char *>(&vertex.color.x), sizeof(float) * 3);
        file.write(reinterpret_cast<const char *>(&vertex.texCoord.x), sizeof(float) * 2);
    }
    for (uint16_t index : mesh->GetIndices())
    {
        file.write(reinterpret_cast<const char *>(&index), sizeof(uint16_t));
    }
    file.close();

    POKE_CORE_INFO("[MeshSerializer] Binary mesh written to: {0}", binaryPath.string());
    return true;
}

std::shared_ptr<Asset> MeshSerializer::DeserializeFromLibrary(const AssetMetaData &metadata, const std::filesystem::path &binaryPath) const
{
    std::ifstream file(binaryPath, std::ios::binary);

    if (!file.is_open())
    {
        POKE_CORE_ERROR("[MeshSerializer] Failed to open: {0}", binaryPath.string());
        return nullptr;
    }

    MeshHeader header;

    file.read(reinterpret_cast<char *>(&header), sizeof(header));

    std::vector<Vertex> vertices(header.vertexCount);
    std::vector<uint16_t> indices(header.indexCount);
    for (Vertex &vertex : vertices)
    {
        file.read(reinterpret_cast<char *>(&vertex.pos.x), sizeof(float) * 3);
        file.read(reinterpret_cast<char *>(&vertex.color.x), sizeof(float) * 3);
        file.read(reinterpret_cast<char *>(&vertex.texCoord.x), sizeof(float) * 2);
    }
    for (uint16_t &index : indices)
    {
        file.read(reinterpret_cast<char *>(&index), sizeof(uint16_t));
    }
    file.close();

    return std::make_shared<Mesh>(metadata.handle, vertices, indices);
}