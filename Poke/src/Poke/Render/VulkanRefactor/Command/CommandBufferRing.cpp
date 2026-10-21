#include "CommandBufferRing.h"
#include "Poke/Core/Assert.h"
#include "../Core/GpuDevice.h"

using namespace Poke;

void CommandBufferRing::Init(GpuDevice *gpu)
{
    m_gpu = gpu;

    for (uint32_t i = 0; i < k_maxPools; ++i)
    {
        VkCommandPoolCreateInfo cmdPoolInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, nullptr};
        cmdPoolInfo.queueFamilyIndex = m_gpu->GetVulkanQueueFamily();
        cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        VkResult result = vkCreateCommandPool(m_gpu->GetVulkanDevice(), &cmdPoolInfo, m_gpu->GetVulkanAllocationCallbacks(), &m_vulkanCommandPools[i]);
        POKE_ASSERT(result == VK_SUCCESS, "Failed to create command pool");
    }

    for (uint32_t i = 0; i < k_maxBuffers; ++i)
    {
        VkCommandBufferAllocateInfo cmd = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
        const uint32_t poolIndex = GetPoolFromIndex(i);
        cmd.commandPool = m_vulkanCommandPools[poolIndex];
        cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmd.commandBufferCount = 1;

        VkResult result = vkAllocateCommandBuffers(m_gpu->GetVulkanDevice(), &cmd, &m_commandBuffers[i].m_commandBuffer);
        POKE_ASSERT(result == VK_SUCCESS, "Failed to allocate command buffers");

        m_commandBuffers[i].m_device = m_gpu;
        m_commandBuffers[i].m_handle = i;
        m_commandBuffers[i].Reset();
    }
}

void CommandBufferRing::Shutdown()
{
    for (uint32_t i = 0; i < kMaxSwapchainImages * k_maxThreads; ++i)
        vkDestroyCommandPool(m_gpu->GetVulkanDevice(), m_vulkanCommandPools[i], m_gpu->GetVulkanAllocationCallbacks());
}

void CommandBufferRing::ResetPools(uint32_t frameIndex)
{
    for (uint32_t i = 0; i < k_maxThreads; ++i)
        vkResetCommandPool(m_gpu->GetVulkanDevice(), m_vulkanCommandPools[frameIndex * k_maxThreads + i], 0);
}

CommandBuffer *CommandBufferRing::GetCommandBuffer(uint32_t frame, bool begin)
{
    CommandBuffer *cb = &m_commandBuffers[frame * k_bufferPerPool];
    if (begin)
    {
        cb->Reset();

        VkCommandBufferBeginInfo beginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(cb->m_commandBuffer, &beginInfo);
    }

    return cb;
}

CommandBuffer *CommandBufferRing::GetCommandBufferInstant(uint32_t frame, bool begin)
{
    CommandBuffer *cb = &m_commandBuffers[frame * k_bufferPerPool + 1];
    return cb;
}

uint16_t CommandBufferRing::GetPoolFromIndex(uint32_t index)
{
    return static_cast<uint16_t>(index) / k_bufferPerPool;
}
