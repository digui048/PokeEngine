#ifndef BUFFER_RESOURCES_H
#define BUFFER_RESOURCES_H

#include "../GpuEnums.h"
#include "../GpuHandles.h"

#include <vulkan/vulkan.h>
#include <cstdint>

#include "../external/vk_mem_alloc.h"

namespace Poke
{
    struct Buffer
    {
        VkBuffer buffer;
        VmaAllocation allocation;
        VkDeviceMemory memory;
        VkDeviceSize size = 0;

        VkBufferUsageFlags typeFlags = 0;
        ResourceUsageType::Enum usage = ResourceUsageType::Immutable;
        uint32_t size = 0;
        uint32_t globalOffset = 0;

        BufferHandle handle;
        BufferHandle parentHandle;

        const char *name = nullptr;
    };

    struct BufferDescription
    {
        void *nativeHandle = nullptr;
        const char *name = nullptr;

        VkBufferUsageFlags typeFlags = 0;
        ResourceUsageType::Enum usage = ResourceUsageType::Immutable;
        uint32_t size = 0;
        BufferHandle parentHandle;
    };
}

#endif