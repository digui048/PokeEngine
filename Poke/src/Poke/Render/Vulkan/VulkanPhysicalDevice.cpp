#include "VulkanPhysicalDevice.h"

#include <vector>

#include "Poke/Core/Log.h"
#include "VulkanInstance.h"

using namespace Poke;

VulkanPhysicalDevice::~VulkanPhysicalDevice()
{
    Shutdown();
}

void VulkanPhysicalDevice::Init(VulkanInstance &instance)
{
    PickPhysicalDevice(instance);

    vkGetPhysicalDeviceProperties(m_physicalDevice, &m_properties);
    vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_features);

    POKE_CORE_INFO("Selected GPU: {0}", m_properties.deviceName);
}

void VulkanPhysicalDevice::Shutdown()
{
    m_physicalDevice = VK_NULL_HANDLE;
}

void VulkanPhysicalDevice::PickPhysicalDevice(VulkanInstance &instance)
{
    uint32_t deviceCount = 0;

    vkEnumeratePhysicalDevices(instance.GetHandle(), &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        POKE_CORE_CRITICAL("Failed to find GPUs wit vulkan support");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);

    vkEnumeratePhysicalDevices(instance.GetHandle(), &deviceCount, devices.data());

    for (VkPhysicalDevice device : devices)
    {
        if (IsDeviceSuitable(device))
        {
            m_physicalDevice = device;
            m_queueFamilies = FindQueueFamilies(device);

            return;
        }
    }

    POKE_CORE_CRITICAL("Failed to find a suitable GPU");
}

bool VulkanPhysicalDevice::IsDeviceSuitable(VkPhysicalDevice device)
{
    QueueFamilyIndices indices = FindQueueFamilies(device);
    return indices.IsComplete();
}

QueueFamilyIndices Poke::VulkanPhysicalDevice::FindQueueFamilies(VkPhysicalDevice device)
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
        if (indices.IsComplete())
        {
            break;
        }
        ++i;
    }

    return indices;
}
