#ifndef ASSET_MANAGER_BASE_H
#define ASSET_MANAGER_BASE_H

#include "Poke/Resources/Asset.h"

#include <memory>

namespace Poke
{
    class AssetManagerBase
    {
    public:
        virtual ~AssetManagerBase() = default;

        virtual std::shared_ptr<Asset> GetAsset(AssetHandle handle) = 0;

        virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
        virtual bool IsAssetLoaded(AssetHandle handle) const = 0;
        virtual AssetType GetAssetType(AssetHandle handle) const = 0;
    };
}

#endif