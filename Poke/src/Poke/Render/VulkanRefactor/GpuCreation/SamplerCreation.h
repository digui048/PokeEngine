#ifndef SAMPLER_CREATION_H
#define SAMPLER_CREATION_H

#include <vulkan/vulkan.h>

namespace Poke
{
    struct SamplerCreation
    {
        VkFilter minFilter = VK_FILTER_NEAREST;
        VkFilter magFilter = VK_FILTER_NEAREST;
        VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;

        VkSamplerAddressMode addressU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode addressV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode addressW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        const char *name = nullptr;

        SamplerCreation &SetMinMagMip(VkFilter min, VkFilter mag, VkSamplerMipmapMode mip);
        SamplerCreation &SetAddressModeU(VkSamplerAddressMode u);
        SamplerCreation &SetAddressModeUV(VkSamplerAddressMode u, VkSamplerAddressMode v);
        SamplerCreation &SetAddressModeUVW(VkSamplerAddressMode u, VkSamplerAddressMode v, VkSamplerAddressMode w);
        SamplerCreation &SetName(const char *name);
    };
}

#endif