#include "AssetImporter.h"
#include "TextureImporter.h"

using namespace Poke;

std::shared_ptr<Asset> AssetImporter::ImportAsset(const AssetMetaData &metadata)
{
    switch (metadata.type)
    {
    case AssetType::Texture:
    {
        return TextureImporter::LoadTexture(metadata);
    }

    default:
        return nullptr;
    }
}