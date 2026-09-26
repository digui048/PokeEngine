#ifndef ASSET_IMPORTER_H
#define ASSET_IMPORTER_H

#include "Poke/Resources/AssetMetaData.h"

namespace Poke
{
    class AssetImporter
    {
    public:
        AssetImporter() = delete;

        static std::shared_ptr<Asset> ImportAsset(const AssetMetaData &metadata);
    };
}

#endif