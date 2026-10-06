#ifndef EDITOR_ASSET_MANAGER_H
#define EDITOR_ASSET_MANAGER_H

#include "Poke/Resources/AssetManagerBase.h"
#include "Poke/Resources/AssetMetaData.h"
#include "Poke/Resources/AssetImportResult.h"
#include "Poke/Serializers/AssetSerializer.h"

#include <unordered_map>
#include <filesystem>
#include <chrono>
#include <vector>
#include <functional>
#include <algorithm>

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
        void RemoveAsset(AssetHandle handle);

        bool IsAssetHandleValid(AssetHandle handle) const override;
        bool IsAssetLoaded(AssetHandle handle) const override;
        AssetType GetAssetType(AssetHandle handle) const override;

        AssetHandle GetAssetHandle(const std::filesystem::path &filePath) const;
        const AssetMetaData &GetMetaData(AssetHandle handle) const;

        std::vector<AssetHandle> GetAssetHandlesInDirectory(const std::filesystem::path &directoryPath) const;

        void ScanDirectoryAssets(const std::filesystem::path &directoryPath);
        AssetImportResult LoadAsset(const AssetMetaData &metadata);
        AssetHandle RegisterAsset(const std::filesystem::path &filePath);
        AssetHandle RegisterGeneratedAsset(const std::shared_ptr<Asset> &asset, const std::filesystem::path &sourcePath, uint32_t generatedAssetIndex);

        bool IsAssetFile(const std::filesystem::path &filePath) const;

        bool SerializeAssetRegistry();
        bool DeserializeAssetRegistry();

    private:
        bool ImportAndSerializeAsset(AssetHandle handle);
        bool SerializeAssetFile(const AssetMetaData &metadata, const std::shared_ptr<Asset> &asset = nullptr) const;

        void InvalidateAsset(AssetHandle handle);
        void RemoveMissingAssets();

        std::filesystem::path GetProjectAssetPath(const std::filesystem::path &sourcePath, AssetType type) const;
        std::filesystem::path GetBinaryPath(const AssetMetaData &metadata) const;
        bool IsAssetExtension(const std::filesystem::path &extension) const;
        std::string GetAssetExtensionFromType(AssetType type) const;
        AssetType GetAssetTypeFromExtension(const std::filesystem::path &extension) const;

        uint64_t GetFileLastWriteTime(const std::filesystem::path &filePath) const;
        bool IsAssetModified(const AssetMetaData &metadata) const;
        bool IsAssetSourceMissing(const AssetMetaData &metadata) const;

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