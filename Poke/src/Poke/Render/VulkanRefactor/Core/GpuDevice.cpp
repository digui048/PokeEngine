#include "GpuDevice.h"
#include "Poke/Core/Log.h"
#include "Poke/Core/Assert.h"

#include <SDL3/SDL_vulkan.h>

using namespace Poke;

static const char *s_requestedLayers[] = {"VK_LAYER_KHRONOS_validation"};
static const char *s_requestedExtensions[] = {VK_KHR_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME};

static SDL_Window *s_sdlWindow;

PFN_vkSetDebugUtilsObjectNameEXT pfnSetDebugUtilsObjectNameEXT;
PFN_vkCmdBeginDebugUtilsLabelEXT pfnCmdBeginDebugUtilsLabelEXT;
PFN_vkCmdEndDebugUtilsLabelEXT pfnCmdEndDebugUtilsLabelEXT;

static size_t s_uboAlignment = 256;
static size_t s_ssboAlignment = 256;

static VkBool32 debugUtilsCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT types, const VkDebugUtilsMessengerCallbackDataEXT *callbackData, void *userData)
{
    if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        POKE_CORE_ERROR("[VulkanValidation] {0}", callbackData->pMessage);
    else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        POKE_CORE_WARN("[VulkanValidation] {0}", callbackData->pMessage);
    else
        POKE_CORE_INFO("[VulkanValidation] {0}", callbackData->pMessage);

    return VK_FALSE;
}

VkDebugUtilsMessengerCreateInfoEXT createDebugUtilsMessengerInfo()
{
    VkDebugUtilsMessengerCreateInfoEXT creationInfo = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    creationInfo.pfnUserCallback = debugUtilsCallback;
    creationInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    creationInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;

    return creationInfo;
}

bool GpuDevice::FindQueueFamily(VkPhysicalDevice device)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_vulkanPhysicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_vulkanPhysicalDevice, &queueFamilyCount, queueFamilies.data());

    uint32_t familyIndex;
    VkBool32 surfaceSupported;
    for (familyIndex = 0; familyIndex < queueFamilyCount; ++familyIndex)
    {
        VkQueueFamilyProperties queueFamily = queueFamilies[familyIndex];
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT))
        {
            vkGetPhysicalDeviceSurfaceSupportKHR(m_vulkanPhysicalDevice, familyIndex, m_vulkanWindowSurface, &surfaceSupported);
            if (surfaceSupported)
            {
                m_vulkanQueueFamily = familyIndex;
                break;
            }
        }
    }

    return surfaceSupported;
}

void GpuDevice::Init(const DeviceCreation &creation)
{
    POKE_CORE_INFO("[GpuDevice] Gpu Device initializing");

    VkResult result;

    VkApplicationInfo applicationInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO, nullptr, "Poke Graphics Device", 1, "PokeEngine", 1, VK_MAKE_VERSION(1, 2, 0)};
    VkInstanceCreateInfo createInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, nullptr, 0, &applicationInfo, sizeof(s_requestedLayers) / sizeof((s_requestedLayers)[0]), s_requestedLayers, sizeof(s_requestedExtensions) / sizeof((s_requestedExtensions)[0]), s_requestedExtensions};
    const VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = createDebugUtilsMessengerInfo();
    createInfo.pNext = &debugCreateInfo;
    result = vkCreateInstance(&createInfo, m_vulkanAllocationCallbacks, &m_vulkanInstance);
    POKE_ASSERT(result == VK_SUCCESS, "Failed to Create vulkan instance");

    m_swapchainWidth = creation.width;
    m_swapchainHeight = creation.height;

    uint32_t numInstanceExtensions;
    vkEnumerateInstanceExtensionProperties(nullptr, &numInstanceExtensions, nullptr);
    std::vector<VkExtensionProperties> extensions(numInstanceExtensions);
    vkEnumerateInstanceExtensionProperties(nullptr, &numInstanceExtensions, extensions.data());
    for (size_t i = 0; i < numInstanceExtensions; ++i)
    {
        if (!std::strcmp(extensions[i].extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        {
            m_debugUtilsExtensionPresent = true;
            break;
        }
    }
    if (!m_debugUtilsExtensionPresent)
        POKE_CORE_INFO("[GpuDevice] Extension {0} for debugging not present", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    else
    {
        PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_vulkanInstance, "vkCreateDebugUtilsMessengerEXT");
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = createDebugUtilsMessengerInfo();

        vkCreateDebugUtilsMessengerEXT(m_vulkanInstance, &debugMessengerCreateInfo, m_vulkanAllocationCallbacks, &m_vulkanDebugMessenger);
    }

    uint32_t numPhysicalDevice;
    result = vkEnumeratePhysicalDevices(m_vulkanInstance, &numPhysicalDevice, NULL);
    POKE_ASSERT(result == VK_SUCCESS, "Failed to enumerate physical devices");
    std::vector<VkPhysicalDevice> gpus(numPhysicalDevice);
    result = vkEnumeratePhysicalDevices(m_vulkanInstance, &numPhysicalDevice, gpus.data());
    POKE_ASSERT(result == VK_SUCCESS, "Failed to enumerate physical devices");

    SDL_Window *window = (SDL_Window *)creation.window;
    if (!SDL_Vulkan_CreateSurface(window, m_vulkanInstance, nullptr, &m_vulkanWindowSurface))
        POKE_CORE_ERROR("[GpuDevice] Failed to create Vulkan surface");

    s_sdlWindow = window;

    VkPhysicalDevice discreteGpu = VK_NULL_HANDLE;
    VkPhysicalDevice integratedGpu = VK_NULL_HANDLE;
    for (uint32_t i = 0; i < numPhysicalDevice; ++i)
    {
        VkPhysicalDevice physicalDevice = gpus[i];
        vkGetPhysicalDeviceProperties(physicalDevice, &m_vulkanPhysicalDeviceProperties);

        if (m_vulkanPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            if (FindQueueFamily(m_vulkanPhysicalDevice))
            {
                discreteGpu = physicalDevice;
                break;
            }
            continue;
        }

        if (m_vulkanPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            if (FindQueueFamily(m_vulkanPhysicalDevice))
                integratedGpu = physicalDevice;

            continue;
        }
    }

    if (discreteGpu != VK_NULL_HANDLE)
        m_vulkanPhysicalDevice = discreteGpu;
    else if (integratedGpu != VK_NULL_HANDLE)
        m_vulkanPhysicalDevice = integratedGpu;
    else
    {
        POKE_ASSERT(true, "[GpuDevice] Suitable Gpu Device not found");
        return;
    }

    vkGetPhysicalDeviceProperties(m_vulkanPhysicalDevice, &m_vulkanPhysicalDeviceProperties);

    s_uboAlignment = m_vulkanPhysicalDeviceProperties.limits.minUniformBufferOffsetAlignment;
    s_ssboAlignment = m_vulkanPhysicalDeviceProperties.limits.minStorageBufferOffsetAlignment;

    uint32_t deviceExtensionCount = 1;
    const char *deviceExtensions[] = {"VK_KHR_swapchain"};
    const float queuePriority[] = {1.0f};
    VkDeviceQueueCreateInfo queueInfo[1] = {};
    queueInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo[0].queueFamilyIndex = m_vulkanQueueFamily;
    queueInfo[0].queueCount = 1;
    queueInfo[0].pQueuePriorities = queuePriority;

    VkPhysicalDeviceFeatures2 physicalFeatures2 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
    vkGetPhysicalDeviceFeatures2(m_vulkanPhysicalDevice, &physicalFeatures2);

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = sizeof(queueInfo) / sizeof(queueInfo[0]);
    deviceCreateInfo.pQueueCreateInfos = queueInfo;
    deviceCreateInfo.enabledExtensionCount = deviceExtensionCount;
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions;
    deviceCreateInfo.pNext = &physicalFeatures2;

    result = vkCreateDevice(m_vulkanPhysicalDevice, &deviceCreateInfo, m_vulkanAllocationCallbacks, &m_vulkanDevice);
    POKE_ASSERT(result == VK_SUCCESS, "Failed to create logical device");

    if (m_debugUtilsExtensionPresent)
    {
        pfnSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr(m_vulkanDevice, "vkSetDebugUtilsObjectNameEXT");
        pfnCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetDeviceProcAddr(m_vulkanDevice, "vkCmdBeginDebugUtilsLabelEXT");
        pfnCmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetDeviceProcAddr(m_vulkanDevice, "vkCmdEndDebugUtilsLabelEXT");
    }

    vkGetDeviceQueue(m_vulkanDevice, m_vulkanQueueFamily, 0, &m_vulkanQueue);
}