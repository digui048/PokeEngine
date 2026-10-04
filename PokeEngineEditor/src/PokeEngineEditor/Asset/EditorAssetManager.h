#ifndef EDITOR_ASSET_MANAGER_H
#define EDITOR_ASSET_MANAGER_H

#include "Poke/Resources/AssetManagerBase.h"
#include "Poke/Resources/AssetMetaData.h"
#include "Poke/Resources/AssetImportResult.h"
#include "Poke/Serializers/AssetSerializer.h"

#include <unordered_map>
#include <filesystem>

namespace Poke
{
    using AssetRegistry = std::unordered_map<AssetHandle, AssetMetaData>;
    using AssetMap = std::unordered_map<AssetHandle, std::shared_ptr<Asset>>;
    using SerializerMap = std::unordered_map<AssetType, std::unique_ptr<AssetSerializer>>;

    class EditorAssetManager : public AssetManagerBase
    {
    public:
        EditorAssetManager(const std::filesystem::path &projectDirectory);
        ~EditorAssetManager() override = default;

        std::shared_ptr<Asset> GetAsset(AssetHandle handle) override;

        bool IsAssetHandleValid(AssetHandle handle) const override;
        bool IsAssetLoaded(AssetHandle handle) const override;
        AssetType GetAssetType(AssetHandle handle) const override;
        AssetHandle GetAssetHandle(const std::filesystem::path &filePath) const;

        const AssetMetaData &GetMetaData(AssetHandle handle) const;

        void ScanDirectoryAssets(const std::filesystem::path &directoryPath);
        AssetImportResult LoadAsset(const AssetMetaData &metadata);
        AssetHandle RegisterAsset(const std::filesystem::path &filePath);
        AssetHandle RegisterSubAsset(const std::shared_ptr<Asset> &asset, AssetHandle parentHandle, uint32_t subAssetIndex);

        bool SerializeAssetRegistry();
        bool DeserializeAssetRegistry();

    private:
        bool ImportAndSerializeAsset(const AssetMetaData &metadata);
        std::filesystem::path GetBinaryPath(const AssetMetaData &metadata) const;
        bool IsAssetExtension(const std::filesystem::path &extension) const;
        AssetType GetAssetTypeFromExtension(const std::filesystem::path &extension) const;

    private:
        AssetRegistry m_assetRegistry;
        AssetMap m_loadedAssets;
        SerializerMap m_assetSerializers;

    private:
        std::filesystem::path m_registryPath = "Assets/AssetRegistry.json";
        std::filesystem::path m_libraryPath = "Library";
    };
}

#endif