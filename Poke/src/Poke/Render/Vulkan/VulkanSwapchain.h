#ifndef VULKAN_SWAPCHAIN_H
#define VULKAN_SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <vector>

namespace Poke
{
    class VulkanDevice;
    class VulkanPhysicalDevice;
    class VulkanSurface;
    class Window;

    class VulkanSwapchain
    {
    public:
        VulkanSwapchain() = default;
        ~VulkanSwapchain();

        void Init(VulkanDevice &device, VulkanPhysicalDevice &physicalDevice, VulkanSurface &surface, Window &window);
        void Shutdown(VulkanDevice &device);

        VkSwapchainKHR GetHandle() const { return m_swapchain; }
        VkFormat GetImageFormat() const { return m_imageFormat; }
        VkExtent2D GetExtent() const { return m_extent; }
        const std::vector<VkImage> &GetImages() const { return m_images; }

    private:
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capablities, Window& window);

    private:
        VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
        std::vector<VkImage> m_images;
        VkFormat m_imageFormat;
        VkExtent2D m_extent;
    };
}

#endif