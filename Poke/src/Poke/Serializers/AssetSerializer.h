#ifndef ASSET_SERIALIZER_H
#define ASSET_SERIALIZER_H

#include "Poke/Resources/Asset.h"
#include "Poke/Resources/AssetMetaData.h"

#include <memory>

namespace Poke
{
    class AssetSerializer
    {
    public:
        virtual ~AssetSerializer() = default;

        virtual bool SerializeToLibrary(const AssetMetaData& metadata, const std::shared_ptr<Asset>& asset, const std::filesystem::path& binaryPath) const = 0;
        virtual std::shared_ptr<Asset> DeserializeFromLibrary(const AssetMetaData& metadata, const std::filesystem::path& binaryPath) const = 0;
    };
}

#endif