#include "VulkanSync.h"

#include "VulkanDevice.h"
#include "Poke/Core/Log.h"

using namespace Poke;

void VulkanSync::Init(VulkanDevice &device)
{
    VkDevice logicalDevice = device.GetHandle();

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &m_imageAvailableSemaphore) != VK_SUCCESS || vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &m_renderFinishedSemaphore) != VK_SUCCESS || vkCreateFence(logicalDevice, &fenceInfo, nullptr, &m_inFlightFence) != VK_SUCCESS)
    {
        POKE_CORE_ERROR("[Vulkan] Failed to create synchronization");
        return;
    }

    POKE_CORE_INFO("[Vulkan] Sync objects created successfully");
}

void VulkanSync::Shutdown(VulkanDevice &device)
{
    VkDevice logicalDevice = device.GetHandle();

    if (m_renderFinishedSemaphore != VK_NULL_HANDLE)
        vkDestroySemaphore(logicalDevice, m_renderFinishedSemaphore, nullptr);
    if (m_imageAvailableSemaphore != VK_NULL_HANDLE)
        vkDestroySemaphore(logicalDevice, m_imageAvailableSemaphore, nullptr);
    if (m_inFlightFence != VK_NULL_HANDLE)
        vkDestroyFence(logicalDevice, m_inFlightFence, nullptr);
}