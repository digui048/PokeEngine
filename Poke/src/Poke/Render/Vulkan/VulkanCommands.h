#ifndef VULKAN_COMMANDS_H
#define VULKAN_COMMANDS_H

#include <vulkan/vulkan.h>

namespace Poke
{
    class VulkanDevice;
    class VulkanPhysicalDevice;
    class VulkanSwapchain;
    class VulkanRenderPass;
    class VulkanFramebuffer;
    class VulkanPipeline;

    class VulkanCommands
    {
    public:
        VulkanCommands() = default;
        ~VulkanCommands() = default;

        void Init(VulkanDevice& device, VulkanPhysicalDevice& physicalDevice);
        void Shutdown(VulkanDevice &device);

        void RecordCommandBuffer(VulkanSwapchain &swapchain, VulkanRenderPass &renderPass, VulkanFramebuffer &framebuffers, VulkanPipeline &pipeline, uint32_t imageIndex);

        VkCommandBuffer GetCommandBuffer() const { return m_commandBuffer; }

    private:
        VkCommandPool m_commandPool = VK_NULL_HANDLE;
        VkCommandBuffer m_commandBuffer = VK_NULL_HANDLE;
    };
}

#endif