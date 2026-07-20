#ifndef VULKAN_PHYSICAL_DEVICE_H
#define VULKAN_PHYSICAL_DEVICE_H

#include <vulkan/vulkan.h>
#include <vector>

#include "QueueFamilyIndices.h"

namespace Poke
{
    class VulkanInstance;
    class VulkanSurface;

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;
    };

    class VulkanPhysicalDevice
    {
    public:
        VulkanPhysicalDevice() = default;
        ~VulkanPhysicalDevice();

        void Init(VulkanInstance &instance, VulkanSurface &surface);
        void Shutdown();

        VkPhysicalDevice GetHandle() const { return m_physicalDevice; }
        const QueueFamilyIndices &GetQueueFamilies() const { return m_queueFamilies; }
        const SwapChainSupportDetails &GetSwapChainSupport() const { return m_swapChainSupport; }

        const VkPhysicalDeviceProperties &GetProperties() const { return m_properties; }
        const VkPhysicalDeviceFeatures &GetFeatures() const { return m_features; }

        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VulkanSurface &surface);

    private:
        void PickPhysicalDevice(VulkanInstance &instance, VulkanSurface &surface);

        bool IsDeviceSuitable(VkPhysicalDevice device, VulkanSurface &surface);

        bool CheckDeviceExtensionsSuport(VkPhysicalDevice device);

        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VulkanSurface &surface);

    private:
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        QueueFamilyIndices m_queueFamilies;
        SwapChainSupportDetails m_swapChainSupport;

        VkPhysicalDeviceProperties m_properties{};
        VkPhysicalDeviceFeatures m_features{};

        const std::vector<const char *> m_deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    };
}

#endif