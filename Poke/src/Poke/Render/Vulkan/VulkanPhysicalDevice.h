#ifndef VULKAN_PHYSICAL_DEVICE_H
#define VULKAN_PHYSICAL_DEVICE_H

#include <vulkan/vulkan.h>

#include "QueueFamilyIndices.h"

namespace Poke
{
    class VulkanInstance;
    class VulkanSurface;

    class VulkanPhysicalDevice
    {
    public:
        VulkanPhysicalDevice() = default;
        ~VulkanPhysicalDevice();

        void Init(VulkanInstance &instance, VulkanSurface &surface);
        void Shutdown();

        VkPhysicalDevice GetHandle() const { return m_physicalDevice; }
        const QueueFamilyIndices &GetQueueFamilies() const { return m_queueFamilies; }

        const VkPhysicalDeviceProperties &GetProperties() const { return m_properties; }
        const VkPhysicalDeviceFeatures &GetFeatures() const { return m_features; }

    private:
        void PickPhysicalDevice(VulkanInstance &instance, VulkanSurface &surface);

        bool IsDeviceSuitable(VkPhysicalDevice device, VulkanSurface &surface);

        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VulkanSurface &surface);

    private:
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        QueueFamilyIndices m_queueFamilies;

        VkPhysicalDeviceProperties m_properties{};
        VkPhysicalDeviceFeatures m_features{};
    };
}

#endif