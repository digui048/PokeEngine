#include "VulkanInstance.h"

#include "Poke/Core/Log.h"
#include <SDL3/SDL_vulkan.h>

#include "VulkanValidation.h"

using namespace Poke;

VulkanInstance::~VulkanInstance()
{
}

void VulkanInstance::Init()
{
    CreateInstance();
}

void VulkanInstance::Shutdown()
{
    if (m_instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
        POKE_CORE_INFO("[Vulkan] Destroying Vulkan Instance");
    }
}

std::vector<const char *> VulkanInstance::GetRequiredExtensions() const
{
    uint32_t extensionCount = 0;

    const char* const* sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);

    if (sdlExtensions == nullptr)
    {
        POKE_CORE_CRITICAL("Failed to get required vulkan extensions");
    }

    std::vector<const char*> extensions(sdlExtensions, sdlExtensions + extensionCount);

    if (VulkanValidation::IsEnabled())
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void VulkanInstance::CreateInstance()
{
    VkApplicationInfo appInfo = VkApplicationInfo();
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "PokeEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.pEngineName = "PokeEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo = VkInstanceCreateInfo();
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = GetRequiredExtensions();

    if (extensions.empty())
    {
        POKE_CORE_CRITICAL("Failed to resolve Vulkan instance extensions");
    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (VulkanValidation::IsEnabled())
    {
        if (VulkanValidation::CheckValidationLayerSupport())
        {
            auto& layers = VulkanValidation::GetValidationLayers();
            createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
            createInfo.ppEnabledLayerNames = layers.data();

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = VkDebugUtilsMessengerCreateInfoEXT();

            VulkanValidation::PopulateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = &debugCreateInfo;
        }
        else
        {
            POKE_CORE_WARN("Validation layers requested but unavailable; creating Vulkan instance without them");
            createInfo.enabledLayerCount = 0;
            createInfo.ppEnabledLayerNames = nullptr;
        }
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
    {
        POKE_CORE_CRITICAL("Failed to create Vulkan Instance");
    }
}