#include "AssetImporter.h"
#include "TextureImporter.h"
#include "ModelImporter.h"

using namespace Poke;

AssetImportResult AssetImporter::ImportAsset(const AssetMetaData &metadata)
{
    switch (metadata.type)
    {
    case AssetType::Texture:
    {
        AssetImportResult result;
        result.asset = TextureImporter::LoadTexture(metadata);
        return result;
    }

    case AssetType::Model:
    {
        return ModelImporter::LoadModel(metadata);
    }

    default:
        return {};
    }
}