#include "VulkanSync.h"

#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "Poke/Core/Log.h"

using namespace Poke;

void VulkanSync::Init(VulkanDevice &device, VulkanSwapchain &swapchain)
{
    VkDevice logicalDevice = device.GetHandle();

    m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    uint32_t swapchainImageCount = static_cast<uint32_t>(swapchain.GetImagesViews().size());
    m_renderFinishedSemaphores.resize(swapchainImageCount);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS || vkCreateFence(logicalDevice, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
        {
            POKE_CORE_ERROR("[Vulkan] Failed to create synchronization");
            return;
        }
    }

    for (size_t i = 0; i < swapchainImageCount; ++i)
    {
        if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS)
        {
            POKE_CORE_ERROR("[Vulkan] Failed to create render finished semaphore for image {0}!", i);
            return;
        }
    }

    POKE_CORE_INFO("[Vulkan] Sync objects created successfully");
}

void VulkanSync::Shutdown(VulkanDevice &device)
{
    VkDevice logicalDevice = device.GetHandle();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        if (m_imageAvailableSemaphores[i] != VK_NULL_HANDLE)
            vkDestroySemaphore(logicalDevice, m_imageAvailableSemaphores[i], nullptr);
        if (m_inFlightFences[i] != VK_NULL_HANDLE)
            vkDestroyFence(logicalDevice, m_inFlightFences[i], nullptr);
    }

    for (size_t i = 0; i < m_renderFinishedSemaphores.size(); ++i)
    {
        if (m_renderFinishedSemaphores[i] != VK_NULL_HANDLE)
            vkDestroySemaphore(logicalDevice, m_renderFinishedSemaphores[i], nullptr);
    }
}