#include "Texture.h"
#include <cstring>

#include "Poke/Render/Vulkan/VulkanTexture.h"

using namespace Poke;

Texture::Texture(const void *pixels, uint32_t width, uint32_t height)
    : Texture(AssetHandle(), pixels, width, height)
{
}

Texture::Texture(AssetHandle handle, const void *pixels, uint32_t width, uint32_t height)
    : Asset(handle), m_width(width), m_height(height)
{
    size_t imageSize = static_cast<size_t>(width) * height * 4;

    if (pixels && imageSize > 0)
    {
        m_pixels.resize(imageSize);
        memcpy(m_pixels.data(), pixels, imageSize);
    }

    m_vulkanTexture = std::make_unique<VulkanTexture>(pixels, width, height);
}

Texture::~Texture()
{
    m_vulkanTexture.reset();
}

ImTextureID Texture::GetImGuiTextureID() const
{
    return m_vulkanTexture->GetImGuiTextureID();
}
