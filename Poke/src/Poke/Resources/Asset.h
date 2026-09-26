#ifndef ASSET_H
#define ASSET_H

#include "Poke/Core/UUID.h"

namespace Poke
{
    using AssetHandle = UUID;

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
        Asset() = default;
        explicit Asset(AssetHandle handle) : m_handle(handle) {}
        virtual ~Asset() = default;

        AssetHandle GetHandle() const { return m_handle; }

        virtual AssetType GetType() const = 0;

    private:
        AssetHandle m_handle;
    };
}

#endif