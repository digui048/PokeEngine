#ifndef DESCRIPTOR_SET_CREATION_H
#define DESCRIPTOR_SET_CREATION_H

#include "../GpuEnums.h"
#include "../GpuHandles.h"
#include "../GpuLimits.h"

#include <vulkan/vulkan.h>
#include <cstdint>

namespace Poke
{
    struct DescriptorSetLayoutCreation
    {
        struct Binding
        {
            VkDescriptorType type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
            uint16_t start = 0;
            uint16_t count = 0;
            const char *name = nullptr;
        };

        Binding bindings[kMaxDescriptorPerSet];
        uint32_t bindingCount = 0;
        uint32_t setIndex = 0;

        const char *name = nullptr;

        DescriptorSetLayoutCreation &Reset();
        DescriptorSetLayoutCreation &AddBinding(const Binding &binding);
        DescriptorSetLayoutCreation &SetSetIndex(uint32_t index);
        DescriptorSetLayoutCreation &SetName(const char *name);
    };

    struct DescriptorSetCreation
    {
        ResourceHandle resources[kMaxDescriptorPerSet];
        SamplerHandle samplers[kMaxDescriptorPerSet];
        uint16_t bindings[kMaxDescriptorPerSet];

        DescriptorSetLayoutHandle layout;
        uint32_t resourceCount = 0;

        const char *name = nullptr;

        DescriptorSetCreation &Reset();
        DescriptorSetCreation &SetLayout(DescriptorSetLayoutHandle layout);
        DescriptorSetCreation &Texture(TextureHandle texture, uint16_t binding);
        DescriptorSetCreation &Buffer(BufferHandle buffer, uint16_t binding);
        DescriptorSetCreation &TextureSampler(TextureHandle texture, SamplerHandle sampler, uint16_t binding);
        DescriptorSetCreation &SetName(const char *name);
    };
}

#endif