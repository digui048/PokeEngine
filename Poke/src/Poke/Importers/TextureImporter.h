#ifndef TEXTURE_IMPORTER_H
#define TEXTURE_IMPORTER_H

#include "Poke/Resources/Assets/Texture.h"
#include "Poke/Resources/AssetMetaData.h"

#include <string>
#include <memory>

namespace Poke
{
    class TextureImporter
    {
    public:
        TextureImporter() = delete;

        static std::shared_ptr<Texture> LoadTexture(const AssetMetaData &metadata);        
        static std::shared_ptr<Texture> LoadTexture(const std::string &filepath);
        static std::shared_ptr<Texture> LoadTexture(AssetHandle handle, const std::string &filepath);
    };
}

#endif