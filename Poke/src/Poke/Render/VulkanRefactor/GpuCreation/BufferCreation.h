#ifndef BUFFER_CREATION_H
#define BUFFER_CREATION_H

#include "../GpuEnums.h"

#include <vulkan/vulkan.h>
#include <cstdint>

namespace Poke
{
    struct BufferCreation
    {
        VkBufferUsageFlags typeFlags = 0;
        ResourceUsageType::Enum usage = ResourceUsageType::Immutable;
        uint32_t size = 0;
        const void *initialData = nullptr;

        const char *name = nullptr;

        BufferCreation &Reset();
        BufferCreation &Set(VkBufferUsageFlags flags, ResourceUsageType::Enum usage, uint32_t size);
        BufferCreation &SetData(void *data);
        BufferCreation &SetName(const char *name);
    };
}

#endif