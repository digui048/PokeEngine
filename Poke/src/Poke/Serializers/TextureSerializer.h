#ifndef TEXTURE_SERIALIZER_H
#define TEXTURE_SERIALIZER_H

#include "Poke/Serializers/AssetSerializer.h"

namespace Poke
{
    struct TextureHeader
    {
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t pixelDataSize = 0;
    };

    class TextureSerializer : public AssetSerializer
    {
    public:
        TextureSerializer() = default;
        ~TextureSerializer() override = default;
        
        bool SerializeToLibrary(const AssetMetaData& metadata, const std::shared_ptr<Asset>& asset, const std::filesystem::path& binaryPath) const override;
        std::shared_ptr<Asset> DeserializeFromLibrary(const AssetMetaData& metadata, const std::filesystem::path& binaryPath) const override;
    };
}

#endif