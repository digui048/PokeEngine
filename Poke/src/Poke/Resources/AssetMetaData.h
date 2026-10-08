#ifndef ASSET_META_DATA_H
#define ASSET_META_DATA_H

#include "Poke/Resources/Asset.h"

#include <filesystem>
#include <limits>

namespace Poke
{
    struct AssetMetaData
    {
        AssetHandle handle;
        AssetType type = AssetType::None;
        std::filesystem::path filePath;
        std::filesystem::path sourcePath;
        AssetHandle parentHandle = 0;
        uint32_t subAssetIndex = std::numeric_limits<uint32_t>::max();

        uint64_t lastWriteTime = 0;

        AssetMetaData() = default;

        operator bool() const
        {
            return static_cast<uint64_t>(handle) != 0 && type != AssetType::None;
        }

        bool IsSubAsset() const
        {
            return static_cast<uint64_t>(parentHandle) != 0;
        }

        static const AssetMetaData Null;
    };

    inline const AssetMetaData AssetMetaData::Null{};
}

#endif