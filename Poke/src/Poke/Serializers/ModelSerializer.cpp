#include "ModelSerializer.h"
#include "Poke/Utils/BinarySerializer.h"

#include "Poke/Core/Log.h"

using namespace Poke;

bool ModelSerializer::SerializeToLibrary(const AssetMetaData &metadata, const std::shared_ptr<Asset> &asset, const std::filesystem::path &binaryPath) const
{
    if (!asset)
    {
        POKE_CORE_ERROR("[ModelSerializer] Can not serialize null asset for handle {0}", static_cast<uint64_t>(metadata.handle));
        return false;
    }

    auto model = std::static_pointer_cast<Model>(asset);

    std::filesystem::create_directories(binaryPath.parent_path());
    std::ofstream file(binaryPath, std::ios::binary);

    if (!file.is_open())
    {
        POKE_CORE_ERROR("[ModelSerializer] Failed to open binary path: {0}", binaryPath.string());
        return false;
    }

    WriteNode(file, model->GetRootNode());

    return true;
}

std::shared_ptr<Asset> ModelSerializer::DeserializeFromLibrary(const AssetMetaData &metadata, const std::filesystem::path &binaryPath) const
{
    std::ifstream file(binaryPath, std::ios::binary);

    if (!file.is_open())
    {
        POKE_CORE_ERROR("[ModelSerializer] Failed to open binary path: {0}", binaryPath.string());
        return nullptr;
    }

    Model::Node rootNode = ReadNode(file);

    if (!file.is_open())
    {
        POKE_CORE_ERROR("[ModelSerializer] Failed reading: {0}", binaryPath.string());
        return nullptr;
    }

    return std::make_shared<Model>(metadata.handle, std::move(rootNode));
}

void ModelSerializer::WriteNode(std::ofstream &file, const Model::Node &node) const
{
    WriteString(file, node.name);

    for (int c = 0; c < 4; ++c)
    {
        for (int r = 0; r < 4; ++r)
        {
            float value = node.localTransform[c][r];
            file.write(reinterpret_cast<char *>(&value), sizeof(float));
        }
    }

    uint32_t meshCount = static_cast<uint32_t>(node.meshes.size());
    file.write(reinterpret_cast<char *>(&meshCount), sizeof(uint32_t));

    for (AssetHandle handle : node.meshes)
    {
        uint64_t value = static_cast<uint64_t>(handle);
        file.write(reinterpret_cast<char *>(&value), sizeof(uint64_t));
    }

    uint32_t childCount = static_cast<uint32_t>(node.children.size());
    file.write(reinterpret_cast<char *>(&childCount), sizeof(uint32_t));

    for (const Model::Node &child : node.children)
    {
        WriteNode(file, child);
    }
}

Model::Node ModelSerializer::ReadNode(std::ifstream &file) const
{
    Model::Node node;
    node.name = ReadString(file);

    for (int c = 0; c < 4; ++c)
    {
        for (int r = 0; r < 4; ++r)
        {
            float value = 0.0f;
            file.read(reinterpret_cast<char *>(&value), sizeof(float));
            node.localTransform[c][r] = value;
        }
    }

    uint32_t meshCount = 0;
    file.read(reinterpret_cast<char *>(&meshCount), sizeof(uint32_t));
    node.meshes.resize(meshCount);

    for (AssetHandle &handle : node.meshes)
    {
        uint64_t value = 0;
        file.read(reinterpret_cast<char *>(&value), sizeof(uint64_t));
        handle = AssetHandle(value);
    }

    uint32_t childCount = 0;
    file.read(reinterpret_cast<char *>(&childCount), sizeof(uint32_t));
    node.children.resize(childCount);

    for (Model::Node &child : node.children)
    {
        child = ReadNode(file);
    }

    return node;
}
