#ifndef VULKAN_VALIDATION_H
#define VULKAN_VALIDATION_H

#include <vector>
#include <array>

#include <vulkan/vulkan.h>

namespace Poke
{
    class VulkanInstance;

    class VulkanValidation
    {
    public:
        VulkanValidation() = default;
        ~VulkanValidation();

        void Init(VulkanInstance &instance);
        void Shutdown(VulkanInstance &instance);

        static bool IsEnabled();

        static const std::array<const char *, 1> &GetValidationLayers();

        static bool CheckValidationLayerSupport();

        static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    public:
        static constexpr std::array<const char*, 1> ValidationLayers = {"VK_LAYER_KHRONOS_validation"};

    private:
        void SetupDebugMessenger(VulkanInstance &instance);

        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);

        static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);

        static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);

    private:
        VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
    };
}

#endif