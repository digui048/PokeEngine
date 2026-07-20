#include "VulkanDevice.h"

#include "VulkanPhysicalDevice.h"
#include "VulkanValidation.h"
#include "Poke/Core/Log.h"

#include <set>

using namespace Poke;

VulkanDevice::~VulkanDevice()
{
}

void VulkanDevice::Init(VulkanInstance &instance, VulkanPhysicalDevice &physicalDevice)
{
    CreateLogicalDevice(instance, physicalDevice);
}

void VulkanDevice::Shutdown()
{
    if (m_device != VK_NULL_HANDLE)
    {
        vkDestroyDevice(m_device, nullptr);
        m_device = VK_NULL_HANDLE;
        m_graphicsQueue = VK_NULL_HANDLE;
        m_presentQueue = VK_NULL_HANDLE;
        POKE_CORE_INFO("[Vulkan] Destroying Vulkan Device");
    }
}

void VulkanDevice::CreateLogicalDevice(VulkanInstance &instance, VulkanPhysicalDevice &physicalDevice)
{
    const QueueFamilyIndices &indices = physicalDevice.GetQueueFamilies();

    std::set<uint32_t> uniqueQueueFamilies = {indices.GraphicsFamily.value(), indices.PresentFamily.value()};
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (VulkanValidation::IsEnabled())
    {
        if (VulkanValidation::CheckValidationLayerSupport())
        {
            auto &layers = VulkanValidation::GetValidationLayers();
            createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
            createInfo.ppEnabledLayerNames = layers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
            createInfo.ppEnabledLayerNames = nullptr;
        }
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
    }

    if (vkCreateDevice(physicalDevice.GetHandle(), &createInfo, nullptr, &m_device) != VK_SUCCESS)
    {
        POKE_CORE_CRITICAL("Failed to create logical device");
    }

    vkGetDeviceQueue(m_device, indices.GraphicsFamily.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, indices.PresentFamily.value(), 0, &m_presentQueue);
}
