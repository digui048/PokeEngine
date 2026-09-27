#include "EditorAssetManager.h"

#include "Poke/Importers/AssetImporter.h"
#include "Poke/Serializers/TextureSerializer.h"
#include "Poke/Core/Log.h"

using namespace Poke;

EditorAssetManager::EditorAssetManager()
{
    m_assetSerializers[AssetType::Texture] =std::make_unique<TextureSerializer>();

    DeserializeAssetRegistry();
}

std::shared_ptr<Asset> EditorAssetManager::GetAsset(AssetHandle handle)
{
    if (IsAssetLoaded(handle))
        return m_loadedAssets.at(handle);

    if (!IsAssetHandleValid(handle))
        return nullptr;

    const AssetMetaData &metadata = m_assetRegistry.at(handle);
    std::filesystem::path binaryPath = GetBinaryPath(metadata);

    std::shared_ptr<Asset> asset = nullptr;

    if (std::filesystem::exists(binaryPath))
    {
        auto it = m_assetSerializers.find(metadata.type);
        if (it != m_assetSerializers.end())
        {
            asset = it->second->DeserializeFromLibrary(metadata, binaryPath);
        }
    }

    if (!asset)
    {
        asset = LoadAsset(metadata);
        if (asset)
        {
            auto it = m_assetSerializers.find(metadata.type);
            if (it != m_assetSerializers.end())
            {
                it->second->SerializeToLibrary(metadata, asset, binaryPath);
            }
        }
        else
        {
            return nullptr;
        }
    }

    if (asset)
    {
        m_loadedAssets[handle] = asset;
    }

    return asset;
}

bool EditorAssetManager::IsAssetHandleValid(AssetHandle handle) const
{
    return m_assetRegistry.find(handle) != m_assetRegistry.end();
}

bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
{
    return m_loadedAssets.find(handle) != m_loadedAssets.end();
}

AssetType EditorAssetManager::GetAssetType(AssetHandle handle) const
{
    if (IsAssetHandleValid(handle))
        return m_assetRegistry.at(handle).type;

    return AssetType::None;
}

const AssetMetaData &EditorAssetManager::GetMetaData(AssetHandle handle) const
{
    if (IsAssetHandleValid(handle))
        return m_assetRegistry.at(handle);

    return AssetMetaData::Null;
}

void EditorAssetManager::ScanDirectoryAssets(const std::filesystem::path &directoryPath)
{
}

std::shared_ptr<Asset> EditorAssetManager::LoadAsset(const AssetMetaData &metadata)
{
    return AssetImporter::ImportAsset(metadata);
}

AssetHandle EditorAssetManager::RegisterAsset(const std::filesystem::path &filePath)
{
    AssetMetaData metadata;
    metadata.handle = AssetHandle();
    metadata.filePath = filePath;
    metadata.type = GetAssetTypeFromExtension(filePath.extension());

    m_assetRegistry[metadata.handle] = metadata;

    POKE_INFO("[AssetManager] Registered asset [{0}] -> {1}", static_cast<uint64_t>(metadata.handle), filePath.string());

    return metadata.handle;
}

void EditorAssetManager::SerializeAssetRegistry()
{
}

void EditorAssetManager::DeserializeAssetRegistry()
{
}

std::filesystem::path EditorAssetManager::GetBinaryPath(const AssetMetaData &metadata) const
{
    std::string extension;
    switch (metadata.type)
    {
        case AssetType::Texture:    extension = ".tex";  break;
        case AssetType::Mesh:       extension = ".mesh"; break;
        case AssetType::Material:   extension = ".tex";  break;
        default:                    extension = ".bin";  break;
    }

    return m_libraryPath / (std::to_string(static_cast<uint64_t>(metadata.handle)) + extension);
}

bool EditorAssetManager::IsAssetExtension(const std::filesystem::path &extension) const
{
    return extension == ".png" || extension == ".jpg" || extension == ".fbx";
}

AssetType EditorAssetManager::GetAssetTypeFromExtension(const std::filesystem::path &extension) const
{
    if (extension == ".png" || extension == ".jpg") return AssetType::Texture;
    if (extension == ".fbx")                        return AssetType::Mesh;

    return AssetType::None;
}
