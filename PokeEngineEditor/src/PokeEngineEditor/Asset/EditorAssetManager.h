#ifndef EDITOR_ASSET_MANAGER_H
#define EDITOR_ASSET_MANAGER_H

#include "Poke/Resources/AssetManagerBase.h"
#include "Poke/Resources/AssetMetaData.h"

#include <unordered_map>
#include <filesystem>

namespace Poke
{
    using AssetRegistry = std::unordered_map<AssetHandle, AssetMetaData>;

    using AssetMap = std::unordered_map<AssetHandle, std::shared_ptr<Asset>>;

    class EditorAssetManager : public AssetManagerBase
    {
    public:
        std::shared_ptr<Asset> GetAsset(AssetHandle handle) override;

        bool IsAssetHandleValid(AssetHandle handle) const override;
        bool IsAssetLoaded(AssetHandle handle) const override;
        AssetType GetAssetType(AssetHandle handle) const override;

        void ImportAsset(const std::filesystem::path &filePath);

        const AssetMetaData &GetMetaData(AssetHandle handle) const;

        void SerializeAssetRegistry();
        void DeserializeAssetRegistry();

    private:
        AssetRegistry m_assetRegistry;
        AssetMap m_loadedAssets;
    };
}

#endif