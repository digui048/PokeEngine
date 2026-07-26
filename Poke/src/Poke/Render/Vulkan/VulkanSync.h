#ifndef VULKAN_SYNC_H
#define VULKAN_SYNC_H

#include <vulkan/vulkan.h>
#include <vector>

namespace Poke
{
    class VulkanDevice;
    class VulkanSwapchain;

    class VulkanSync
    {
    public:
        static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

        VulkanSync() = default;
        ~VulkanSync() = default;

        void Init(VulkanDevice &device, VulkanSwapchain &swapchain);
        void Shutdown(VulkanDevice &device);

        VkSemaphore GetImageAvailableSemaphore(uint32_t frameIndex) const { return m_imageAvailableSemaphores[frameIndex]; }
        VkSemaphore GetRenderFinishedSemaphore(uint32_t frameIndex) const { return m_renderFinishedSemaphores[frameIndex]; }
        VkFence GetInFlightFence(uint32_t frameIndex) const { return m_inFlightFences[frameIndex]; }

    private:
        std::vector<VkSemaphore> m_imageAvailableSemaphores;
        std::vector<VkSemaphore> m_renderFinishedSemaphores;
        std::vector<VkFence> m_inFlightFences;
    };
}

#endif