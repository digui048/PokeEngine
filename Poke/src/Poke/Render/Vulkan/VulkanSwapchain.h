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

        void Init(VulkanDevice &device, VulkanPhysicalDevice &physicalDevice, VulkanSurface &surface, Window &window, VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE);
        void Shutdown(VulkanDevice &device);

        VkSwapchainKHR GetHandle() const { return m_swapchain; }
        VkFormat GetImageFormat() const { return m_imageFormat; }
        VkExtent2D GetExtent() const { return m_extent; }
        const std::vector<VkImage> &GetImages() const { return m_images; }
        const std::vector<VkImageView> &GetImagesViews() const { return m_imageViews; }

    private:
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capablities, Window &window);

        void CreateImageViews(VkDevice logicalDevice);

    private:
        VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
        std::vector<VkImage> m_images;
        std::vector<VkImageView> m_imageViews;
        VkFormat m_imageFormat;
        VkExtent2D m_extent;
    };
}

#endif