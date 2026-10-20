#ifndef DESCRIPTOR_SET_RESOURCE_H
#define DESCRIPTOR_SET_RESOURCE_H

#include "../Core/GpuHandles.h"
#include "../Core/GpuLimits.h"

#include <vulkan/vulkan.h>
#include <cstdint>

namespace Poke
{
    struct DescriptorSetLayout;

    struct ResourceData
    {
        void *data = nullptr;
    };

    struct ResourceBinding
    {
        uint16_t type = 0;
        uint16_t start = 0;
        uint16_t count = 0;
        uint16_t set = 0;

        const char *name = nullptr;
    };

    struct DescriptorBinding
    {
        VkDescriptorType type;
        uint16_t start = 0;
        uint16_t count = 0;
        uint16_t set = 0;

        const char *name = nullptr;
    };

    struct DescriptorSetLayout
    {
        VkDescriptorSetLayout layout;

        VkDescriptorSetLayoutBinding *binding = nullptr;
        DescriptorBinding *bindings = nullptr;
        uint16_t numBindings = 0;
        uint16_t setIndex = 0;

        DescriptorSetLayoutHandle handle;
    };

    struct DescriptorSet
    {
        VkDescriptorSet set;

        ResourceHandle *resources = nullptr;
        SamplerHandle *samplers = nullptr;
        uint16_t *bindings = nullptr;

        const DescriptorSetLayout *layout = nullptr;
        uint32_t resourceCount = 0;
    };

    struct DescriptorSetLayoutDescription
    {
        ResourceBinding bindings[kMaxDescriptorPerSet];
        uint32_t activeBindingsCount = 0;
    };

    struct DescriptorSetDescription
    {
        ResourceData resources[kMaxDescriptorPerSet];
        uint32_t activeResourcesCount = 0;
    };
}

#endif