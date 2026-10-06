#include "EditorAssetManager.h"

#include "Poke/Resources/Assets/Model.h"
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

    if (IsAssetLoaded(handle))
        return m_loadedAssets.at(handle);

    const AssetMetaData &metadata = m_assetRegistry.at(handle);

    if (!metadata.sourcePath.empty())
    {
        if (IsAssetSourceMissing(metadata))
        {
            POKE_CORE_WARN("[AssetManager] Source file missing: {0}", metadata.sourcePath.string());

            RemoveAsset(handle);
            return nullptr;
        }

        if (IsAssetModified(metadata))
        {
            POKE_CORE_WARN("[AssetManager] Source asset modified: {0}", metadata.sourcePath.string());

            InvalidateAsset(handle);

            if (!ImportAndSerializeAsset(handle))
                return nullptr;

            if (IsAssetLoaded(handle))
                return m_loadedAssets.at(handle);
        }
    }

    const AssetMetaData &currentMetadata = m_assetRegistry.at(handle);

    auto serializerIt = m_assetSerializers.find(currentMetadata.type);
    if (serializerIt == m_assetSerializers.end())
        return nullptr;

    std::filesystem::path binaryPath = GetBinaryPath(currentMetadata);

    if (std::filesystem::exists(binaryPath))
    {
        std::shared_ptr<Asset> asset = serializerIt->second->DeserializeFromLibrary(currentMetadata, binaryPath);

        if (asset)
        {
            m_loadedAssets[handle] = asset;
            return asset;
        }
    }

    if (!currentMetadata.sourcePath.empty())
    {
        if (!ImportAndSerializeAsset(handle))
            return nullptr;

        auto it = m_loadedAssets.find(handle);
        if (it != m_loadedAssets.end())
            return it->second;
    }

    POKE_CORE_ERROR("[AssetManager] Could not load asset: {0}");

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
        if (metadata.filePath == filePath || metadata.sourcePath == filePath)
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

std::vector<AssetHandle> EditorAssetManager::GetAssetHandlesInDirectory(const std::filesystem::path &directoryPath) const
{
    std::vector<AssetHandle> handles;

    for (const auto &[handle, metadata] : m_assetRegistry)
    {
        if (metadata.filePath.parent_path() == directoryPath)
            handles.push_back(handle);
    }

    std::sort(handles.begin(), handles.end(), [this](AssetHandle left, AssetHandle right) {
        return m_assetRegistry.at(left).filePath.filename().string() < m_assetRegistry.at(right).filePath.filename().string();
    });

    return handles;
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
            if (metadata.sourcePath == filePath)
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
    metadata.sourcePath = filePath;
    metadata.type = GetAssetTypeFromExtension(filePath.extension());

    if (metadata.type == AssetType::None)
        return AssetHandle();

    metadata.filePath = GetProjectAssetPath(filePath, metadata.type);
    metadata.lastWriteTime = GetFileLastWriteTime(filePath);

    m_assetRegistry[metadata.handle] = metadata;

    if (!SerializeAssetFile(metadata))
        return AssetHandle();

    POKE_CORE_INFO("[AssetManager] Registered asset [{0}] -> {1}", static_cast<uint64_t>(metadata.handle), metadata.filePath.string());

    return metadata.handle;
}

AssetHandle EditorAssetManager::RegisterGeneratedAsset(const std::shared_ptr<Asset> &asset, const std::filesystem::path &sourcePath, uint32_t generatedAssetIndex)
{
    if (!asset)
        return AssetHandle();

    AssetMetaData metadata;
    metadata.handle = asset->GetHandle();
    metadata.type = asset->GetType();
    metadata.sourcePath.clear();
    metadata.lastWriteTime = 0;

    std::filesystem::path modelPath = GetProjectAssetPath(sourcePath, AssetType::Model);

    std::string filename = modelPath.stem().string() + "_" + std::to_string(generatedAssetIndex) + GetAssetExtensionFromType(metadata.type);
    metadata.filePath = modelPath.parent_path() / filename;

    m_assetRegistry[metadata.handle] = metadata;
    m_loadedAssets[metadata.handle] = asset;

    POKE_CORE_INFO("[AssetManager] Registered generated [{0}] -> parent {1}", static_cast<uint64_t>(metadata.handle), metadata.filePath.string());

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
        item.Set("SourcePath", metadata.sourcePath.string());
        item.Set("FilePath", metadata.filePath.string());
        item.Set("LastWriteTime", metadata.lastWriteTime);

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
        metadata.sourcePath = std::filesystem::path(item.Get<std::string>("SourcePath", ""));
        metadata.filePath = std::filesystem::path(item.Get<std::string>("FilePath", ""));
        metadata.lastWriteTime = item.Get<uint64_t>("LastWriteTime", 0);

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

    for (uint32_t i = 0; i < result.generatedAssets.size(); ++i)
    {
        const auto &generatedAsset = result.generatedAssets[i];
        AssetHandle generatedHandle = RegisterGeneratedAsset(generatedAsset, initialMetadata.sourcePath, i);

        auto serializerIt = m_assetSerializers.find(generatedAsset->GetType());
        if (serializerIt == m_assetSerializers.end())
            return false;

        const AssetMetaData &generatedMetadata = m_assetRegistry.at(generatedHandle);

        if (!serializerIt->second->SerializeToLibrary(generatedMetadata, generatedAsset, GetBinaryPath(generatedMetadata)))
            return false;

        if (!SerializeAssetFile(generatedMetadata))
            return false;
    }

    auto serializerIt = m_assetSerializers.find(initialMetadata.type);
    if (serializerIt == m_assetSerializers.end())
        return false;

    const AssetMetaData &metadata = m_assetRegistry.at(handle);

    if (!serializerIt->second->SerializeToLibrary(metadata, result.asset, GetBinaryPath(metadata)))
        return false;

    if (!SerializeAssetFile(metadata, result.asset))
        return false;

    m_loadedAssets[metadata.handle] = result.asset;

    AssetMetaData &updatedMetadata = m_assetRegistry.at(handle);
    updatedMetadata.lastWriteTime = GetFileLastWriteTime(updatedMetadata.sourcePath);

    SerializeAssetRegistry();

    return true;
}

bool EditorAssetManager::SerializeAssetFile(const AssetMetaData &metadata, const std::shared_ptr<Asset> &asset) const
{
    JsonNode root;
    JsonNode assetNode;

    assetNode.Set("Handle", static_cast<uint64_t>(metadata.handle));
    assetNode.Set("Type", AssetTypeToString(metadata.type));
    assetNode.Set("SourcePath", metadata.sourcePath.string());

    if (metadata.type == AssetType::Model && asset)
    {
        std::shared_ptr<Model> model = std::static_pointer_cast<Model>(asset);
        auto &meshes = assetNode.GetInternal()["Meshes"] = nlohmann::json::array();

        std::function<void(const Model::Node &)> collectMeshes;
        collectMeshes = [&](const Model::Node &node)
        {
            for (AssetHandle meshHandle : node.meshes)
            {
                meshes.push_back(static_cast<uint64_t>(meshHandle));
            }

            for (const Model::Node &child : node.children)
            {
                collectMeshes(child);
            }
        };

        collectMeshes(model->GetRootNode());
    }

    root.GetInternal()["Asset"] = assetNode.GetInternal();
    std::filesystem::create_directories(metadata.filePath.parent_path());

    return Json::SaveToFile(root, metadata.filePath);
}

void EditorAssetManager::InvalidateAsset(AssetHandle handle)
{
    if (!IsAssetHandleValid(handle))
        return;

    const AssetMetaData metadata = m_assetRegistry.at(handle);

    m_loadedAssets.erase(handle);

    std::filesystem::remove(metadata.filePath);
    std::filesystem::remove(GetBinaryPath(metadata));
}

void EditorAssetManager::RemoveAsset(AssetHandle handle)
{
    if (!IsAssetHandleValid(handle))
        return;

    const AssetMetaData metadata = m_assetRegistry.at(handle);

    m_loadedAssets.erase(handle);

    std::filesystem::remove(metadata.filePath);
    std::filesystem::remove(GetBinaryPath(metadata));

    POKE_CORE_INFO("[AssetManager] Remove asset with type: {0} and path {1}", AssetTypeToString(metadata.type), metadata.sourcePath.string());

    m_assetRegistry.erase(handle);

    SerializeAssetRegistry();
}

void EditorAssetManager::RemoveMissingAssets()
{
    std::vector<AssetHandle> missingAssets;

    for (const auto &[handle, metadata] : m_assetRegistry)
    {
        if (metadata.sourcePath.empty())
            continue;

        if (!std::filesystem::exists(metadata.sourcePath))
            missingAssets.push_back(handle);
    }

    for (AssetHandle handle : missingAssets)
    {
        const auto it = m_assetRegistry.find(handle);
        if (it == m_assetRegistry.end())
            continue;

        POKE_CORE_WARN("[AssetManager] Source file removed: {0}", it->second.sourcePath.string());

        RemoveAsset(handle);
    }
}

std::filesystem::path EditorAssetManager::GetProjectAssetPath(const std::filesystem::path &sourcePath, AssetType type) const
{
    std::filesystem::path path = sourcePath;
    switch (type)
    {
    case AssetType::Texture:
        path.replace_extension(".tex");
        break;

    case AssetType::Model:
        path.replace_extension(".model");
        break;

    case AssetType::Material:
        path.replace_extension(".mat");
        break;

    case AssetType::Mesh:
        path.replace_extension(".mesh");
        break;

    default:
        break;
    }

    return path;
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

std::string EditorAssetManager::GetAssetExtensionFromType(AssetType type) const
{
    std::string extension;
    switch (type)
    {
    case AssetType::Model:
        extension = ".model";
        break;

    case AssetType::Texture:
        extension = ".tex";
        break;

    case AssetType::Material:
        extension = ".mat";
        break;

    case AssetType::Mesh:
        extension = ".mesh";
        break;

    default:
        extension = "";
        break;
    }

    return extension;
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
    if (metadata.sourcePath.empty())
        return false;

    if (!std::filesystem::exists(metadata.sourcePath))
        return false;

    uint64_t currentLastWriteTime = GetFileLastWriteTime(metadata.sourcePath);

    return currentLastWriteTime != metadata.lastWriteTime;
}

bool EditorAssetManager::IsAssetSourceMissing(const AssetMetaData &metadata) const
{
    if (metadata.sourcePath.empty())
        return true;

    return !std::filesystem::exists(metadata.sourcePath);
}
