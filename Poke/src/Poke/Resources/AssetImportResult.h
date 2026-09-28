#ifndef ASSET_IMPORT_RESULT_H
#define ASSET_IMPORT_RESULT_H

#include "Poke/Resources/Asset.h"

#include <memory>
#include <vector>

namespace Poke
{
    struct AssetImportResult
    {
        std::shared_ptr<Asset> asset;
        std::vector<std::shared_ptr<Asset>> subAssets;
    };
}

#endif