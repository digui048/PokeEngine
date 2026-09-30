#ifndef MESH_SERIALIZER_H
#define MESH_SERIALIZER_H

#include "Poke/Serializers/AssetSerializer.h"

namespace Poke
{
    struct MeshHeader
    {
        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;
    };

    class MeshSerializer : public AssetSerializer
    {
    public:
        MeshSerializer() = default;
        ~MeshSerializer() override = default;

        bool SerializeToLibrary(const AssetMetaData &metadata, const std::shared_ptr<Asset> &asset, const std::filesystem::path &binaryPath) const override;
        std::shared_ptr<Asset> DeserializeFromLibrary(const AssetMetaData &metadata, const std::filesystem::path &binaryPath) const override;
    };
}

#endif