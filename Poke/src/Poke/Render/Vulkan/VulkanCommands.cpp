#include "VulkanCommands.h"

#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSync.h"
#include "Poke/Core/Log.h"

using namespace Poke;

void VulkanCommands::Init(VulkanDevice &device, VulkanPhysicalDevice &physicalDevice)
{
    VkDevice logicalDevice = device.GetHandle();
    QueueFamilyIndices queueFamilyIndices = physicalDevice.GetQueueFamilies();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();

    if (vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
    {
        POKE_CORE_ERROR("[Vulkan] Failed to create command pool!");
        return;
    }

    m_commandBuffers.resize(VulkanSync::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

    if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
    {
        POKE_CORE_ERROR("[Vulkan] Failed to allocate command buffer!");
        return;
    }

    POKE_CORE_INFO("[Vulkan] Command Pool and Buffer created successfully");
}

void VulkanCommands::Shutdown(VulkanDevice &device)
{
    if (m_commandPool != VK_NULL_HANDLE)
    {
        vkDestroyCommandPool(device.GetHandle(), m_commandPool, nullptr);
        m_commandPool = VK_NULL_HANDLE;
    }
}