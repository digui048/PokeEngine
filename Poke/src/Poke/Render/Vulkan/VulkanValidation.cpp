#include "VulkanValidation.h"

#include "VulkanInstance.h"
#include "Poke/Core/Log.h"

using namespace Poke;

VulkanValidation::~VulkanValidation()
{
}

void VulkanValidation::Init(VulkanInstance &instance)
{
    if (!IsEnabled())
    {
        return;
    }

    SetupDebugMessenger(instance);
}

void VulkanValidation::Shutdown(VulkanInstance &instance)
{
    if (!IsEnabled())
    {
        return;
    }

    if (m_debugMessenger != VK_NULL_HANDLE)
    {
        DestroyDebugUtilsMessengerEXT(instance.GetHandle(), m_debugMessenger, nullptr);

        m_debugMessenger = VK_NULL_HANDLE;
    }
}

bool VulkanValidation::IsEnabled()
{
#ifdef NDEBUG
    return false;
#else
    return true;
#endif
}

const std::array<const char *, 1> &VulkanValidation::GetValidationLayers()
{
    return ValidationLayers;
}

bool VulkanValidation::CheckValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : ValidationLayers)
    {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

void VulkanValidation::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    createInfo.pfnUserCallback = DebugCallback;
    createInfo.pUserData = nullptr;
}

void VulkanValidation::SetupDebugMessenger(VulkanInstance &instance)
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo = VkDebugUtilsMessengerCreateInfoEXT();
    PopulateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance.GetHandle(), &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
    {
        POKE_CORE_CRITICAL("Failed to setup vulkan debug messenger");
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL Poke::VulkanValidation::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        POKE_CORE_ERROR("[Vulkan Validation] {0}", pCallbackData->pMessage);
    }
    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        POKE_CORE_WARN("[Vulkan Validation] {0}", pCallbackData->pMessage);
    }
    else
    {
        POKE_CORE_INFO("[Vulkan Validation] {0}", pCallbackData->pMessage);
    }

    return VK_FALSE;
}

VkResult VulkanValidation::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void VulkanValidation::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
{
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}
