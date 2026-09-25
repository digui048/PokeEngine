#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "Poke/Resources/Asset.h"

#include <memory>

namespace Poke
{
    class AssetManager
    {
    public:
        template<typename T>
        static std::shared_ptr<T> GetAsset(AssetHandle handle);

        static bool IsAssetHandleValid(AssetHandle handle);
        static bool IsAssetLoaded(AssetHandle handle);
        static AssetType GetAssetType(AssetHandle handle);
    };
}

#endif