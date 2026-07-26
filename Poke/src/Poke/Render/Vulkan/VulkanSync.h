#ifndef VULKAN_SYNC_H
#define VULKAN_SYNC_H

#include <vulkan/vulkan.h>

namespace Poke
{
    class VulkanDevice;

    class VulkanSync
    {
    public:
        VulkanSync() = default;
        ~VulkanSync() = default;

        void Init(VulkanDevice &device);
        void Shutdown(VulkanDevice &device);

        VkSemaphore GetImageAvailableSemaphore() const { return m_imageAvailableSemaphore; }
        VkSemaphore GetRenderFinishedSemaphore() const { return m_renderFinishedSemaphore; }
        VkFence GetInFlightFence() const { return m_inFlightFence; }

    private:
        VkSemaphore m_imageAvailableSemaphore = VK_NULL_HANDLE;
        VkSemaphore m_renderFinishedSemaphore = VK_NULL_HANDLE;
        VkFence m_inFlightFence = VK_NULL_HANDLE;
    };
}

#endif