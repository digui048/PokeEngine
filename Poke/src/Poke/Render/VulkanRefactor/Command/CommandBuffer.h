#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include <vulkan/vulkan.h>

namespace Poke
{
    class GpuDevice;

    struct CommandBuffer
    {
        VkCommandBuffer m_commandBuffer;

        GpuDevice *m_device;
        uint32_t m_handle;
        
        void Reset();
    };
}

#endif