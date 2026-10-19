#ifndef TEXTURE_CREATION_H
#define TEXTURE_CREATION_H

#include "../GpuEnums.h"

#include <vulkan/vulkan.h>
#include <cstdint>

namespace Poke
{
    struct TextureCreation
    {
        void *initialData = nullptr;
        uint16_t width = 1;
        uint16_t height = 1;
        uint16_t depth = 1;
        uint8_t mipmaps = 1;
        uint8_t flags = 0;

        VkFormat format = VK_FORMAT_UNDEFINED;
        TextureType::Enum type = TextureType::Texture2D;

        const char *name = nullptr;

        TextureCreation& SetSize(uint16_t width, uint16_t height, uint16_t depth);
        TextureCreation& SetFlags(uint8_t mipmaps, uint8_t flags);
        TextureCreation& SetFormatType(VkFormat format, TextureType::Enum type);
        TextureCreation& SetData(void *data);
        TextureCreation& SetName(const char *name);
    };
}

#endif