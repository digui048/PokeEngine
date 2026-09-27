#include "TextureSerializer.h"
#include "Poke/Resources/Assets/Texture.h"
#include "Poke/Core/Log.h"

#include <fstream>
#include <vector>

using namespace Poke;

bool TextureSerializer::SerializeToLibrary(const AssetMetaData &metadata, const std::shared_ptr<Asset> &asset, const std::filesystem::path &binaryPath) const
{
    if (!asset)
    {
        POKE_CORE_ERROR("[TextureSerializer] Can not serialize null asset for handle {0}", static_cast<uint64_t>(metadata.handle));
        return false;
    }

    auto texture = std::static_pointer_cast<Texture>(asset);

    TextureHeader header;
    header.width = texture->GetWidth();
    header.height = texture->GetHeight();
    header.pixelDataSize = texture->GetPixelsSize();

    const uint8_t *pixelData = texture->GetPixels();
    if (!pixelData || header.pixelDataSize == 0)
    {
        POKE_CORE_ERROR("[TextureSerializer] Texture pixels is empty: {0}", metadata.filePath.string());
        return false;
    }

    std::filesystem::create_directories(binaryPath.parent_path());
    std::ofstream file(binaryPath, std::ios::binary);
    if (!file.is_open())
    {
        POKE_CORE_ERROR("[TextureSerializer] Failed to open binary path: {0}", binaryPath.string());
        return false;
    }

    file.write(reinterpret_cast<const char *>(&header), sizeof(TextureHeader));
    file.write(reinterpret_cast<const char *>(pixelData), header.pixelDataSize);
    file.close();

    POKE_CORE_INFO("[TextureSerializer] Binary texture written to: {0}", binaryPath.string());
    return true;
}

std::shared_ptr<Asset> TextureSerializer::DeserializeFromLibrary(const AssetMetaData &metadata, const std::filesystem::path &binaryPath) const
{
    std::ifstream file(binaryPath, std::ios::binary);
    if (!file.is_open())
    {
        POKE_CORE_ERROR("[TextureSerializer] Failed to open binary file for reading: {0}", binaryPath.string());
        return nullptr;
    }

    TextureHeader header;
    file.read(reinterpret_cast<char *>(&header), sizeof(TextureHeader));

    if (!file || header.pixelDataSize == 0)
    {
        POKE_CORE_ERROR("[TextureSerializer] Binary file header is corrupt or invalid: {0}", metadata.filePath.string());
        return nullptr;
    }

    std::vector<uint8_t> pixelData(header.pixelDataSize);
    file.read(reinterpret_cast<char *>(pixelData.data()), header.pixelDataSize);
    file.close();

    return std::make_shared<Texture>(metadata.handle, pixelData.data(), header.width, header.height);
}
