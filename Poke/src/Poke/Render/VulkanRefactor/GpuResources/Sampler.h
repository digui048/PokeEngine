#ifndef SAMPLER_RESOURCE_H
#define SAMPLER_RESOURCE_H

#include <vulkan/vulkan.h>

namespace Poke
{
    struct Sampler
    {
        VkSampler sampler;

        VkFilter minFilter = VK_FILTER_NEAREST;
        VkFilter magFilter = VK_FILTER_NEAREST;
        VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;

        VkSamplerAddressMode adressU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode adressV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode adressW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        const char *name = nullptr;
    };

    struct SamplerDescription
    {
        const char *name = nullptr;

        VkFilter minFilter = VK_FILTER_NEAREST;
        VkFilter magFilter = VK_FILTER_NEAREST;
        VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;

        VkSamplerAddressMode adressU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode adressV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode adressW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    };
}

#endif