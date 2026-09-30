#include "EditorAssetManager.h"

#include "Poke/Importers/AssetImporter.h"
#include "Poke/Serializers/TextureSerializer.h"
#include "Poke/Serializers/ModelSerializer.h"
#include "Poke/Serializers/MeshSerializer.h"
#include "Poke/Utils/Json.h"
#include "Poke/Core/Log.h"

using namespace Poke;

EditorAssetManager::EditorAssetManager(const std::filesystem::path &projectDirectory)
{
    m_registryPath = projectDirectory / "Assets/AssetRegistry.json";
    m_libraryPath = projectDirectory / "Library";

    std::filesystem::create_directories(m_libraryPath);

    m_assetSerializers[AssetType::Texture] = std::make_unique<TextureSerializer>();
    m_assetSerializers[AssetType::Model] = std::make_unique<ModelSerializer>();
    m_assetSerializers[AssetType::Mesh] = std::make_unique<MeshSerializer>();
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
        AssetImportResult result = LoadAsset(metadata);
        asset = result.asset;

        if (!asset)
            return nullptr;

        for (uint32_t i = 0; i < result.subAssets.size(); ++i)
        {
            const auto &subAsset = result.subAssets[i];
            AssetHandle subHandle = RegisterSubAsset(subAsset, handle, i);

            auto it = m_assetSerializers.find(subAsset->GetType());
            if (it != m_assetSerializers.end())
            {
                AssetMetaData subMetadata = m_assetRegistry.at(subHandle);
                it->second->SerializeToLibrary(subMetadata, subAsset, GetBinaryPath(subMetadata));
            }
        }

        auto it = m_assetSerializers.find(metadata.type);
        if (it != m_assetSerializers.end())
        {
            it->second->SerializeToLibrary(metadata, asset, binaryPath);
        }

        SerializeAssetRegistry();
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

AssetHandle EditorAssetManager::GetAssetHandle(const std::filesystem::path &filePath) const
{
    for (const auto &[handle, metadata] : m_assetRegistry)
    {
        if (metadata.filePath == filePath)
            return handle;
    }

    return AssetHandle();
}

const AssetMetaData &EditorAssetManager::GetMetaData(AssetHandle handle) const
{
    if (IsAssetHandleValid(handle))
        return m_assetRegistry.at(handle);

    return AssetMetaData::Null;
}

void EditorAssetManager::ScanDirectoryAssets(const std::filesystem::path &directoryPath)
{
    if (!std::filesystem::exists(directoryPath))
    {
        POKE_CORE_WARN("[AssetManager] Directory does not exist: {0}", directoryPath.string());
        return;
    }

    bool newAssetDiscovered = false;

    for (const auto &entry : std::filesystem::recursive_directory_iterator(directoryPath))
    {
        if (entry.is_directory())
            continue;

        const auto &filePath = entry.path();

        if (filePath.extension() == ".meta" || filePath.filename() == "AssetRegistry.json")
            continue;

        if (!IsAssetExtension(filePath.extension()))
            continue;

        bool alreadyRegistered = false;
        for (const auto &[handle, metadata] : m_assetRegistry)
        {
            if (metadata.filePath == filePath)
            {
                alreadyRegistered = true;
                break;
            }
        }

        if (!alreadyRegistered)
        {
            RegisterAsset(filePath);
            newAssetDiscovered = true;
        }
    }

    if (newAssetDiscovered)
    {
        SerializeAssetRegistry();
    }
}

AssetImportResult EditorAssetManager::LoadAsset(const AssetMetaData &metadata)
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

    POKE_CORE_INFO("[AssetManager] Registered asset [{0}] -> {1}", static_cast<uint64_t>(metadata.handle), filePath.string());

    return metadata.handle;
}

AssetHandle Poke::EditorAssetManager::RegisterSubAsset(const std::shared_ptr<Asset> &asset, AssetHandle parentHandle, uint32_t subAssetIndex)
{
    if (!asset)
        return AssetHandle();

    AssetMetaData metadata;
    metadata.handle = asset->GetHandle();
    metadata.type = asset->GetType();
    metadata.filePath = "";
    metadata.parentHandle = parentHandle;
    metadata.subAssetIndex = subAssetIndex;

    m_assetRegistry[metadata.handle] = metadata;
    m_loadedAssets[metadata.handle] = asset;

    POKE_CORE_INFO("[AssetManager] Registered subasset [{0}] -> parent {1}", static_cast<uint64_t>(metadata.handle), static_cast<uint64_t>(metadata.parentHandle));

    return metadata.handle;
}

bool EditorAssetManager::SerializeAssetRegistry()
{
    JsonNode root;
    JsonNode registryNode;

    auto &assetsArray = registryNode.GetInternal()["Assets"] = nlohmann::json::array();

    for (const auto &[handle, metadata] : m_assetRegistry)
    {
        JsonNode item;
        item.Set("Handle", static_cast<uint64_t>(metadata.handle));
        item.Set("Type", AssetTypeToString(metadata.type));
        item.Set("FilePath", metadata.filePath.string());

        uint64_t parentHandle = static_cast<uint64_t>(metadata.parentHandle);
        if (parentHandle != 0)
        {
            item.Set("ParentHandle", parentHandle);
            item.Set("SubAssetIndex", metadata.subAssetIndex);
        }
        assetsArray.push_back(item.GetInternal());
    }

    root.GetInternal()["AssetRegistry"] = registryNode.GetInternal();

    return Json::SaveToFile(root, m_registryPath);
}

bool EditorAssetManager::DeserializeAssetRegistry()
{
    JsonNode root = Json::LoadFromFile(m_registryPath);
    if (!root.HasKey("AssetRegistry"))
    {
        POKE_CORE_WARN("[EditorAssetManager] Could not find AssetRegistry key in {0}", m_registryPath.string());
        return false;
    }

    JsonNode registryNode = root["AssetRegistry"];
    JsonNode assetsNode = registryNode["Assets"];

    const auto &assetsJson = assetsNode.GetInternal();

    if (!assetsJson.is_array())
    {
        POKE_CORE_WARN("[EditorAssetManager] Assets node is not an array in {0}", m_registryPath.string());
        return false;
    }

    m_assetRegistry.clear();

    for (const auto &itemJson : assetsJson)
    {
        JsonNode item(itemJson);

        AssetMetaData metadata;
        metadata.handle = AssetHandle(item.Get<uint64_t>("Handle", 0));
        metadata.type = AssetTypeFromString(item.Get<std::string>("Type", "None"));
        metadata.filePath = std::filesystem::path(item.Get<std::string>("FilePath", ""));
        metadata.parentHandle = AssetHandle(item.Get<uint64_t>("ParentHandle", 0));

        metadata.subAssetIndex = item.Get<uint32_t>("SubAssetIndex", std::numeric_limits<uint32_t>::max());

        if (metadata)
        {
            m_assetRegistry[metadata.handle] = metadata;
        }
    }

    POKE_CORE_INFO("[EditorAssetManager] Loaded {0} assets from registry", m_assetRegistry.size());
    return true;
}

std::filesystem::path EditorAssetManager::GetBinaryPath(const AssetMetaData &metadata) const
{
    std::filesystem::path directory;
    std::string extension;
    switch (metadata.type)
    {
    case AssetType::Texture:
        directory = "Textures";
        extension = ".tex";
        break;
    case AssetType::Mesh:
        directory = "Meshes";
        extension = ".mesh";
        break;
    case AssetType::Material:
        directory = "Materials";
        extension = ".mat";
        break;
    case AssetType::Model:
        directory = "Models";
        extension = ".model";
        break;
    default:
        directory = "Others";
        extension = ".bin";
        break;
    }

    return m_libraryPath / directory / (std::to_string(static_cast<uint64_t>(metadata.handle)) + extension);
}

bool EditorAssetManager::IsAssetExtension(const std::filesystem::path &extension) const
{
    return extension == ".png" || extension == ".jpg" || extension == ".fbx";
}

AssetType EditorAssetManager::GetAssetTypeFromExtension(const std::filesystem::path &extension) const
{
    if (extension == ".png" || extension == ".jpg")
        return AssetType::Texture;
    if (extension == ".fbx")
        return AssetType::Model;

    return AssetType::None;
}
