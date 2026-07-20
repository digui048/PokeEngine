#include "VulkanPhysicalDevice.h"

#include <vector>
#include <string>
#include <set>

#include "Poke/Core/Log.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"

using namespace Poke;

VulkanPhysicalDevice::~VulkanPhysicalDevice()
{
    Shutdown();
}

void VulkanPhysicalDevice::Init(VulkanInstance &instance, VulkanSurface &surface)
{
    PickPhysicalDevice(instance, surface);

    vkGetPhysicalDeviceProperties(m_physicalDevice, &m_properties);
    vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_features);

    POKE_CORE_INFO("Selected GPU: {0}", m_properties.deviceName);
}

void VulkanPhysicalDevice::Shutdown()
{
    m_physicalDevice = VK_NULL_HANDLE;
}

SwapChainSupportDetails Poke::VulkanPhysicalDevice::QuerySwapChainSupport(VkPhysicalDevice device, VulkanSurface &surface)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface.GetHandle(), &details.Capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.GetHandle(), &formatCount, nullptr);
    if (formatCount != 0)
    {
        details.Formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.GetHandle(), &formatCount, details.Formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.GetHandle(), &presentModeCount, nullptr);
    if (presentModeCount != 0)
    {
        details.PresentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.GetHandle(), &presentModeCount, details.PresentModes.data());
    }

    return details;
}

void VulkanPhysicalDevice::PickPhysicalDevice(VulkanInstance &instance, VulkanSurface &surface)
{
    uint32_t deviceCount = 0;

    vkEnumeratePhysicalDevices(instance.GetHandle(), &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        POKE_CORE_CRITICAL("Failed to find GPUs wit vulkan support");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);

    vkEnumeratePhysicalDevices(instance.GetHandle(), &deviceCount, devices.data());

    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
    int bestScore = -1;
    QueueFamilyIndices bestQueueFamilies;

    for (VkPhysicalDevice device : devices)
    {
        if (IsDeviceSuitable(device, surface))
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);

            int score = 0;
            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                score += 1000;
            }
            else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
            {
                score += 100;
            }

            if (score > bestScore)
            {
                bestDevice = device;
                bestScore = score;
                bestQueueFamilies = FindQueueFamilies(device, surface);
                m_swapChainSupport = QuerySwapChainSupport(device, surface);
            }
        }
    }

    if (bestDevice != VK_NULL_HANDLE)
    {
        m_physicalDevice = bestDevice;
        m_queueFamilies = bestQueueFamilies;
        return;
    }

    POKE_CORE_CRITICAL("Failed to find a suitable GPU");
}

bool VulkanPhysicalDevice::IsDeviceSuitable(VkPhysicalDevice device, VulkanSurface &surface)
{
    QueueFamilyIndices indices = FindQueueFamilies(device, surface);
    bool extensionsSupported = CheckDeviceExtensionsSuport(device);

    bool swapChainCorrect = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);
        swapChainCorrect = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
    }

    return indices.IsComplete() && extensionsSupported && swapChainCorrect;
}

bool Poke::VulkanPhysicalDevice::CheckDeviceExtensionsSuport(VkPhysicalDevice device)
{
    uint32_t extensionsCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionsCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

QueueFamilyIndices Poke::VulkanPhysicalDevice::FindQueueFamilies(VkPhysicalDevice device, VulkanSurface &surface)
{
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    uint32_t i = 0;
    for (const auto &queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.GraphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface.GetHandle(), &presentSupport);
        if (presentSupport)
        {
            indices.PresentFamily = i;
        }

        if (indices.IsComplete())
        {
            break;
        }
        ++i;
    }

    return indices;
}
