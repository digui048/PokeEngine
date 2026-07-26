#ifndef VULKAN_RENDER_PASS_H
#define VULKAN_RENDER_PASS_H

#include <vulkan/vulkan.h>

namespace Poke
{
    class VulkanDevice;
    class VulkanSwapchain;

    class VulkanRenderPass
    {
    public:
        VulkanRenderPass() = default;
        ~VulkanRenderPass() = default;

        void Init(VulkanDevice &device, VulkanSwapchain &swapchain);
        void Shutdown(VulkanDevice &device);

        VkRenderPass GetHandle() const { return m_renderPass; }

    private:
        VkRenderPass m_renderPass = VK_NULL_HANDLE;
    };
}

#endif