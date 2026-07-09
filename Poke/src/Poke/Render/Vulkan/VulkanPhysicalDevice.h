#ifndef VULKAN_PHYSICAL_DEVICE_H
#define VULKAN_PHYSICAL_DEVICE_H

#include <vulkan/vulkan.h>

#include "QueueFamilyIndices.h"

namespace Poke
{
    class VulkanInstance;

    class VulkanPhysicalDevice
    {
    public:
        VulkanPhysicalDevice() = default;
        ~VulkanPhysicalDevice();

        void Init(VulkanInstance &instance);
        void Shutdown();

        VkPhysicalDevice GetHandle() const { return m_physicalDevice; }
        const QueueFamilyIndices& GetQueueFamilies() const { return m_queueFamilies; }

        const VkPhysicalDeviceProperties& GetProperties() const { return m_properties; }
        const VkPhysicalDeviceFeatures& GetFeatures() const { return m_features; }

    private:
        void PickPhysicalDevice(VulkanInstance& instance);
        
        bool IsDeviceSuitable(VkPhysicalDevice device);

        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

    private:
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        QueueFamilyIndices m_queueFamilies;

        VkPhysicalDeviceProperties m_properties{};
        VkPhysicalDeviceFeatures m_features{};
    };
}

#endif