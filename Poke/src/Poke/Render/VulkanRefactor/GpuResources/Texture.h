#ifndef TEXTURE_RESOURCE_H
#define TEXTURE_RESOURCE_H

#include "../GpuEnums.h"
#include "../GpuHandles.h"

#include <vulkan/vulkan.h>
#include <cstdint>

#include "../external/vk_mem_alloc.h"

namespace Poke
{
    struct Sampler;

    struct Texture
    {
        VkImage image;
        VkImageView view;
        VkFormat format;
        VkImageLayout layout;
        VmaAllocation allocation;

        uint16_t width = 1;
        uint16_t height = 1;
        uint16_t depth = 1;
        uint8_t mipmaps = 1;
        uint8_t flags = 0;

        TextureHandle handle;
        TextureType::Enum type = TextureType::Texture2D;

        Sampler *sampler = nullptr;

        const char *name = nullptr;
    };

    struct TextureDescription
    {
        void *nativeHandle = nullptr;
        const char *name = nullptr;

        uint16_t width = 1;
        uint16_t height = 1;
        uint16_t depth = 1;
        uint8_t mipmaps = 1;
        uint8_t renderTarget = 0;
        uint8_t computeAccess = 0;

        VkFormat format = VK_FORMAT_UNDEFINED;
        TextureType::Enum type = TextureType::Texture2D;
    };
}

#endif