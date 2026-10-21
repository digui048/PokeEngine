#ifndef COMMAND_BUFFER_RING_H
#define COMMAND_BUFFER_RING_H

#include "Poke/Render/VulkanRefactor/Core/GpuLimits.h"
#include "CommandBuffer.h"

#include <cstdint>
#include <vulkan/vulkan.h>

namespace Poke
{
    class GpuDevice;
    class CommandBuffer;

    struct CommandBufferRing
    {
        void Init(GpuDevice* gpu);
        void Shutdown();

        void ResetPools(uint32_t frameIndex);

        CommandBuffer* GetCommandBuffer(uint32_t frame, bool begin);
        CommandBuffer* GetCommandBufferInstant(uint32_t frame, bool begin);

        static uint16_t GetPoolFromIndex(uint32_t index);

        static const uint16_t k_maxThreads = 1;
        static const uint16_t k_maxPools = kMaxSwapchainImages * k_maxThreads;
        static const uint16_t k_bufferPerPool = 4;
        static const uint16_t k_maxBuffers = k_bufferPerPool * k_maxPools;
        
        GpuDevice* m_gpu;
        VkCommandPool m_vulkanCommandPools[k_maxPools];
        CommandBuffer m_commandBuffers[k_maxBuffers];
        uint8_t m_nextFreePerThreadFrame[k_maxPools];
    };
}

#endif