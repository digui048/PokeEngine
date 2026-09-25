#ifndef ASSET_META_DATA_H
#define ASSET_META_DATA_H

#include "Poke/Resources/Asset.h"

#include <filesystem>

namespace Poke
{
    struct AssetMetaData
    {
        AssetType type = AssetType::None;
        std::filesystem::path filePath;

        operator bool() const
        {
            return type != AssetType::None;
        }
    };
}

#endif