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
    if (!IsAssetHandleValid(handle))
        return nullptr;

    const AssetMetaData &metadata = m_assetRegistry.at(handle);
    AssetHandle sourceHandle = metadata.IsSubAsset() ? metadata.parentHandle : handle;

    if (!IsAssetHandleValid(sourceHandle))
        return nullptr;

    const AssetMetaData &sourceMetadata = m_assetRegistry.at(sourceHandle);

    if (IsAssetSourceMissing(sourceMetadata))
    {
        POKE_CORE_WARN("[AssetManager] Source file missing: {0}", sourceMetadata.filePath.string());

        RemoveAsset(sourceHandle);
        return nullptr;
    }

    if (IsAssetModified(sourceMetadata))
    {
        POKE_CORE_WARN("[AssetManager] Source asset modified: {0}", sourceMetadata.filePath.string());

        InvalidateAsset(sourceHandle);

        if (!ImportAndSerializeAsset(sourceHandle))
            return nullptr;
    }

    if (!IsAssetHandleValid(handle))
        return nullptr;

    if (IsAssetLoaded(handle))
        return m_loadedAssets.at(handle);

    const AssetMetaData &currentMetadata = m_assetRegistry.at(handle);
    std::filesystem::path binaryPath = GetBinaryPath(currentMetadata);

    auto serializerIt = m_assetSerializers.find(currentMetadata.type);

    if (std::filesystem::exists(binaryPath) && serializerIt != m_assetSerializers.end())
    {
        std::shared_ptr<Asset> asset = serializerIt->second->DeserializeFromLibrary(metadata, binaryPath);

        if (asset)
        {
            m_loadedAssets[handle] = asset;
            return asset;
        }
    }

    if (currentMetadata.IsSubAsset())
    {
        AssetHandle parentHandle = currentMetadata.parentHandle;

        if (!IsAssetHandleValid(parentHandle))
            return nullptr;

        InvalidateAsset(parentHandle);

        if (!ImportAndSerializeAsset(parentHandle))
            return nullptr;

        auto it = m_loadedAssets.find(handle);
        if (it != m_loadedAssets.end())
            return it->second;

        return nullptr;
    }

    if (!ImportAndSerializeAsset(handle))
        return nullptr;

    auto it = m_loadedAssets.find(handle);
    if (it != m_loadedAssets.end())
        return it->second;

    return nullptr;
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

    RemoveMissingAssets();

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
    metadata.lastWriteTime = GetFileLastWriteTime(filePath);

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

bool EditorAssetManager::IsAssetFile(const std::filesystem::path &filePath) const
{
    return IsAssetExtension(filePath.extension());
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
        item.Set("LastWriteTime", metadata.lastWriteTime);

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
        metadata.lastWriteTime = item.Get<uint64_t>("LastWriteTime", 0);
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

bool EditorAssetManager::ImportAndSerializeAsset(AssetHandle handle)
{
    if (!IsAssetHandleValid(handle))
        return false;

    const AssetMetaData initialMetadata = m_assetRegistry.at(handle);
    AssetImportResult result = LoadAsset(initialMetadata);

    if (!result.asset)
        return false;

    for (uint32_t i = 0; i < result.subAssets.size(); ++i)
    {
        const auto &subAsset = result.subAssets[i];
        AssetHandle subHandle = RegisterSubAsset(subAsset, handle, i);

        auto serializerIt = m_assetSerializers.find(subAsset->GetType());
        if (serializerIt == m_assetSerializers.end())
            return false;

        const AssetMetaData &subMetadata = m_assetRegistry.at(subHandle);

        if (!serializerIt->second->SerializeToLibrary(subMetadata, subAsset, GetBinaryPath(subMetadata)))
            return false;
    }

    auto serializerIt = m_assetSerializers.find(initialMetadata.type);
    if (serializerIt == m_assetSerializers.end())
        return false;

    const AssetMetaData &metadata = m_assetRegistry.at(handle);

    if (!serializerIt->second->SerializeToLibrary(metadata, result.asset, GetBinaryPath(metadata)))
        return false;

    m_loadedAssets[metadata.handle] = result.asset;

    AssetMetaData &updatedMetadata = m_assetRegistry.at(handle);
    updatedMetadata.lastWriteTime = GetFileLastWriteTime(updatedMetadata.filePath);

    SerializeAssetRegistry();

    return true;
}

void EditorAssetManager::InvalidateAsset(AssetHandle handle)
{
    if (!IsAssetHandleValid(handle))
        return;

    AssetHandle parentHandle = handle;

    if (m_assetRegistry.at(handle).IsSubAsset())
        parentHandle = m_assetRegistry.at(handle).parentHandle;

    if (!IsAssetHandleValid(parentHandle))
        return;

    m_loadedAssets.erase(parentHandle);

    const AssetMetaData &parentMetadata = m_assetRegistry.at(parentHandle);
    std::filesystem::remove(GetBinaryPath(parentMetadata));

    std::vector<AssetHandle> subAssetHandles;
    for (const auto &[subHandle, metadata] : m_assetRegistry)
    {
        if (metadata.parentHandle == parentHandle)
            subAssetHandles.push_back(subHandle);
    }

    for (AssetHandle subHandle : subAssetHandles)
    {
        auto it = m_assetRegistry.find(subHandle);
        if (it == m_assetRegistry.end())
            continue;

        m_loadedAssets.erase(subHandle);
        std::filesystem::remove(GetBinaryPath(it->second));

        m_assetRegistry.erase(it);
    }

    SerializeAssetRegistry();
}

void EditorAssetManager::RemoveAsset(AssetHandle handle)
{
    if (!IsAssetHandleValid(handle))
        return;

    AssetHandle parentHandle = handle;

    if (m_assetRegistry.at(handle).IsSubAsset())
        parentHandle = m_assetRegistry.at(handle).parentHandle;

    if (!IsAssetHandleValid(parentHandle))
        return;

    std::vector<AssetHandle> subAssetHandles;
    for (const auto &[subHandle, metadata] : m_assetRegistry)
    {
        if (metadata.parentHandle == parentHandle)
            subAssetHandles.push_back(subHandle);
    }

    for (AssetHandle subHandle : subAssetHandles)
    {
        auto it = m_assetRegistry.find(subHandle);
        if (it == m_assetRegistry.end())
            continue;

        m_loadedAssets.erase(subHandle);
        std::filesystem::remove(GetBinaryPath(it->second));

        m_assetRegistry.erase(it);
    }

    m_loadedAssets.erase(parentHandle);

    const AssetMetaData &parentMetadata = m_assetRegistry.at(parentHandle);
    std::filesystem::remove(GetBinaryPath(parentMetadata));

    POKE_CORE_INFO("[AssetManager] Remove asset with type: {0} and path {1}", AssetTypeToString(parentMetadata.type), parentMetadata.filePath.string());

    m_assetRegistry.erase(parentHandle);

    SerializeAssetRegistry();
}

void EditorAssetManager::RemoveMissingAssets()
{
    std::vector<AssetHandle> missingAssets;

    for (const auto &[handle, metadata] : m_assetRegistry)
    {
        if (metadata.IsSubAsset())
            continue;

        if (metadata.filePath.empty())
            continue;

        if (!std::filesystem::exists(metadata.filePath))
            missingAssets.push_back(handle);
    }

    for (AssetHandle handle : missingAssets)
    {
        const auto it = m_assetRegistry.find(handle);
        if (it == m_assetRegistry.end())
            continue;

        POKE_CORE_WARN("[AssetManager] Source file removed: {0}", it->second.filePath.string());

        RemoveAsset(handle);
    }
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

uint64_t EditorAssetManager::GetFileLastWriteTime(const std::filesystem::path &filePath) const
{
    if (!std::filesystem::exists(filePath))
        return 0;

    auto lastWriteTime = std::filesystem::last_write_time(filePath);

    return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(lastWriteTime.time_since_epoch()).count());
}

bool EditorAssetManager::IsAssetModified(const AssetMetaData &metadata) const
{
    if (metadata.IsSubAsset())
    {
        if (!IsAssetHandleValid(metadata.parentHandle))
            return false;

        return IsAssetModified(m_assetRegistry.at(metadata.parentHandle));
    }

    if (metadata.filePath.empty())
        return false;

    if (!std::filesystem::exists(metadata.filePath))
        return false;

    uint64_t currentLastWriteTime = GetFileLastWriteTime(metadata.filePath);

    return currentLastWriteTime != metadata.lastWriteTime;
}

bool EditorAssetManager::IsAssetSourceMissing(const AssetMetaData &metadata) const
{
    if (metadata.IsSubAsset())
    {
        if (!IsAssetHandleValid(metadata.parentHandle))
            return true;

        return IsAssetSourceMissing(m_assetRegistry.at(metadata.parentHandle));
    }

    if (metadata.filePath.empty())
        return true;

    return !std::filesystem::exists(metadata.filePath);
}
