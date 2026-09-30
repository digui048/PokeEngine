#ifndef ASSET_H
#define ASSET_H

#include "Poke/Core/UUID.h"
#include <string>

namespace Poke
{
    using AssetHandle = UUID;

    enum class AssetType : uint16_t
    {
        None = 0,
        Texture,
        Material,
        Mesh,
        Model
    };

    inline std::string AssetTypeToString(AssetType type)
    {
        switch (type)
        {
        case AssetType::Texture:
            return "Texture";
        case AssetType::Model:
            return "Model";
        case AssetType::Mesh:
            return "Mesh";
        default:
            return "None";
        }
    }

    inline AssetType AssetTypeFromString(const std::string &typeStr)
    {
        if (typeStr == "Texture")
            return AssetType::Texture;
        if (typeStr == "Model")
            return AssetType::Model;
        if (typeStr == "Mesh")
            return AssetType::Mesh;
            
        return AssetType::None;
    }

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