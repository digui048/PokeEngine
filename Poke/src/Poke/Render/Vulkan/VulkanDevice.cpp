#include "VulkanDevice.h"

#include "VulkanPhysicalDevice.h"
#include "VulkanValidation.h"
#include "Poke/Core/Log.h"

using namespace Poke;

VulkanDevice::~VulkanDevice()
{
    Shutdown();
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
    }
}

void VulkanDevice::CreateLogicalDevice(VulkanInstance &instance, VulkanPhysicalDevice &physicalDevice)
{
    const QueueFamilyIndices &indices = physicalDevice.GetQueueFamilies();
    float queuePriority = 1.0f;

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.GraphicsFamily.value();
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;
    createInfo.ppEnabledExtensionNames = nullptr;

    if (VulkanValidation::IsEnabled())
    {
        auto& layers = VulkanValidation::GetValidationLayers();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(layers.size());
        createInfo.ppEnabledLayerNames = layers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice.GetHandle(), &createInfo, nullptr, &m_device) != VK_SUCCESS)
    {
        POKE_CORE_CRITICAL("Failed to create logical device");
    }

    vkGetDeviceQueue(m_device, indices.GraphicsFamily.value(), 0, &m_graphicsQueue);
}
