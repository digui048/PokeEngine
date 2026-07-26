#ifndef VULKAN_FRAMEBUFFER
#define VULKAN_FRAMEBUFFER

#include <vulkan/vulkan.h>
#include <vector>

namespace Poke
{
    class VulkanDevice;
    class VulkanSwapchain;
    class VulkanRenderPass;

    class VulkanFramebuffer
    {
    public:
        VulkanFramebuffer() = default;
        ~VulkanFramebuffer() = default;

        void Init(VulkanDevice &device, VulkanSwapchain &swapchain, VulkanRenderPass &renderPass);
        void Shutdown(VulkanDevice &device);

        const std::vector<VkFramebuffer> &GetFramebuffers() const { return m_framebuffers; }
        VkFramebuffer GetFramebuffer(size_t index) const { return m_framebuffers[index]; }

    private:
        std::vector<VkFramebuffer> m_framebuffers;
    };
}

#endif