#ifndef ASSET_IMPORTER_H
#define ASSET_IMPORTER_H

#include "Poke/Resources/AssetImportResult.h"
#include "Poke/Resources/AssetMetaData.h"

namespace Poke
{
    class AssetImporter
    {
    public:
        AssetImporter() = delete;

        static AssetImportResult ImportAsset(const AssetMetaData &metadata);
    };
}

#endif