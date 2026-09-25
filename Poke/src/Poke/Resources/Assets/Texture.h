#ifndef TEXTURE_H
#define TEXTURE_H

#include "Poke/Resources/Asset.h"

#include <vector>
#include <memory>
#include <cstdint>
#include <imgui.h>

namespace Poke
{
    class VulkanTexture;

    class Texture : public Asset
    {
    public:
        Texture(const void *pixels, uint32_t width, uint32_t height);
        ~Texture();

        const uint32_t GetWidth() const { return m_width; }
        const uint32_t GetHeight() const { return m_height; }
        const std::vector<uint8_t> &GetPixels() const { return m_pixels; }

        AssetType GetType() const override { return AssetType::Texture; }

        ImTextureID GetImGuiTextureID() const;
        const VulkanTexture *GetVulkanTexture() const { return m_vulkanTexture.get(); }

    private:
        uint32_t m_width;
        uint32_t m_height;
        std::vector<uint8_t> m_pixels;

        std::unique_ptr<VulkanTexture> m_vulkanTexture;
    };
}

#endif