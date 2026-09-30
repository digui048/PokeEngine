#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "Poke/Resources/Asset.h"
#include "Poke/Resources/AssetManagerBase.h"

#include <memory>

namespace Poke
{
    class AssetManager
    {
    public:
        AssetManager() = delete;

        static void SetActive(AssetManagerBase *manager);
        static void Shutdown();

        static AssetManagerBase *GetActive();

        template <typename T>
        static std::shared_ptr<T> GetAsset(AssetHandle handle)
        {
            AssetManagerBase *manager = GetActive();
            if (!manager)
                return nullptr;

            std::shared_ptr<Asset> asset = manager->GetAsset(handle);

            if (!asset)
                return nullptr;

            return std::static_pointer_cast<T>(asset);
        }

        static bool IsAssetHandleValid(AssetHandle handle);
        static bool IsAssetLoaded(AssetHandle handle);
        static AssetType GetAssetType(AssetHandle handle);

    private:
        static AssetManagerBase *s_activeManager;
    };
}

#endif