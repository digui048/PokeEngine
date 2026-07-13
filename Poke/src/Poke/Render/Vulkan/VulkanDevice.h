#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#include <vulkan/vulkan.h>
#include <vector>

namespace Poke
{
    class VulkanInstance;
    class VulkanPhysicalDevice;

    class VulkanDevice
    {
        public:
            VulkanDevice() = default;
            ~VulkanDevice();

            void Init(VulkanInstance& instance, VulkanPhysicalDevice& physicalDevice);

            void Shutdown();

            VkDevice GetHandle() const { return m_device; }

            VkQueue GetGraphicsQueue() const { return m_graphicsQueue; }
        
        private:
            void CreateLogicalDevice(VulkanInstance& instance, VulkanPhysicalDevice& physicalDevice);

        private:
            VkDevice m_device = VK_NULL_HANDLE;
            VkQueue m_graphicsQueue = VK_NULL_HANDLE;
            VkQueue m_presentQueue = VK_NULL_HANDLE;
    };
}

#endif