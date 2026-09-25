#ifndef ASSET_H
#define ASSET_H

#include "Poke/Core/UUID.h"

namespace Poke
{
    enum class AssetType
    {
        None = 0,
        Texture,
        Material,
        Mesh
    };

    class Asset
    {
    public:
        UUID m_uuid;

        virtual AssetType GetType() const = 0;
    };
}

#endif