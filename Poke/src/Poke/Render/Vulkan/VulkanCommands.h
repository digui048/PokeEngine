#ifndef VULKAN_COMMANDS_H
#define VULKAN_COMMANDS_H

#include <vulkan/vulkan.h>
#include <vector>

namespace Poke
{
    class VulkanDevice;
    class VulkanPhysicalDevice;

    class VulkanCommands
    {
    public:
        VulkanCommands() = default;
        ~VulkanCommands() = default;

        void Init(VulkanDevice& device, VulkanPhysicalDevice& physicalDevice);
        void Shutdown(VulkanDevice &device);

        VkCommandBuffer GetCommandBuffer(uint32_t frameIndex) const { return m_commandBuffers[frameIndex]; }

    private:
        VkCommandPool m_commandPool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> m_commandBuffers;
    };
}

#endif