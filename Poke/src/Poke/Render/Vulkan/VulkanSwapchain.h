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
        VkFormat GetDepthFormat() const { return m_depthFormat; }
        VkImageView GetDepthImageView() const { return m_depthImageView; }

    private:
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capablities, Window &window);

        void CreateImageViews(VkDevice logicalDevice);
        void CreateDepthResources(VulkanDevice &device, VulkanPhysicalDevice &physicalDevice);
        VkFormat FindDepthFormat(VkPhysicalDevice physicalDevice);

    private:
        VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
        std::vector<VkImage> m_images;
        std::vector<VkImageView> m_imageViews;
        VkFormat m_imageFormat;
        VkExtent2D m_extent;

    private:
        VkImage m_depthImage = VK_NULL_HANDLE;
        VkDeviceMemory m_depthImageMemory = VK_NULL_HANDLE;
        VkImageView m_depthImageView = VK_NULL_HANDLE;
        VkFormat m_depthFormat;
    };
}

#endif