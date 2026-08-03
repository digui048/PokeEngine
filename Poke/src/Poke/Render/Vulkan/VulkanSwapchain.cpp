#include "VulkanSwapchain.h"

#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSurface.h"

#include "Poke/Core/Window.h"
#include "Poke/Core/Log.h"

#include <SDL3/SDL.h>
#include <algorithm>
#include <limits>

using namespace Poke;

VulkanSwapchain::~VulkanSwapchain()
{
}

void VulkanSwapchain::Init(VulkanDevice &device, VulkanPhysicalDevice &physicalDevice, VulkanSurface &surface, Window &window, VkSwapchainKHR oldSwapchain)
{
    for (auto imageView : m_imageViews)
    {
        vkDestroyImageView(device.GetHandle(), imageView, nullptr);
    }
    m_imageViews.clear();

    if (m_depthImageView != VK_NULL_HANDLE) {
        vkDestroyImageView(device.GetHandle(), m_depthImageView, nullptr);
        m_depthImageView = VK_NULL_HANDLE;
    }
    if (m_depthImage != VK_NULL_HANDLE) {
        vkDestroyImage(device.GetHandle(), m_depthImage, nullptr);
        m_depthImage = VK_NULL_HANDLE;
    }
    if (m_depthImageMemory != VK_NULL_HANDLE) {
        vkFreeMemory(device.GetHandle(), m_depthImageMemory, nullptr);
        m_depthImageMemory = VK_NULL_HANDLE;
    }

    SwapChainSupportDetails swapChainSupport = physicalDevice.QuerySwapChainSupport(physicalDevice.GetHandle(), surface);

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.Formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.PresentModes);
    VkExtent2D extent = ChooseSwapExtent(swapChainSupport.Capabilities, window);

    uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
    if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.Capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface.GetHandle();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = physicalDevice.GetQueueFamilies();
    uint32_t queueFamilyIndices[] = {indices.GraphicsFamily.value(), indices.PresentFamily.value()};

    if (indices.GraphicsFamily != indices.PresentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = oldSwapchain;

    VkSwapchainKHR newSwapchain = VK_NULL_HANDLE;
    if (vkCreateSwapchainKHR(device.GetHandle(), &createInfo, nullptr, &newSwapchain) != VK_SUCCESS)
    {
        POKE_CORE_CRITICAL("Failed to create swap chain");
    }
    if (oldSwapchain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(device.GetHandle(), oldSwapchain, nullptr);
    }

    m_swapchain = newSwapchain;

    vkGetSwapchainImagesKHR(device.GetHandle(), m_swapchain, &imageCount, nullptr);
    m_images.resize(imageCount);
    vkGetSwapchainImagesKHR(device.GetHandle(), m_swapchain, &imageCount, m_images.data());

    m_imageFormat = surfaceFormat.format;
    m_extent = extent;

    CreateImageViews(device.GetHandle());
    CreateDepthResources(device, physicalDevice);
}

void VulkanSwapchain::Shutdown(VulkanDevice &device)
{
    VkDevice logicalDevice = device.GetHandle();

    if (m_depthImageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(logicalDevice, m_depthImageView, nullptr);
        m_depthImageView = VK_NULL_HANDLE;
    }
    if (m_depthImage != VK_NULL_HANDLE)
    {
        vkDestroyImage(logicalDevice, m_depthImage, nullptr);
        m_depthImage = VK_NULL_HANDLE;
    }
    if (m_depthImageMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(logicalDevice, m_depthImageMemory, nullptr);
        m_depthImageMemory = VK_NULL_HANDLE;
    }

    for (auto imageView : m_imageViews)
    {
        vkDestroyImageView(logicalDevice, imageView, nullptr);
    }
    m_imageViews.clear();

    if (m_swapchain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(device.GetHandle(), m_swapchain, nullptr);
        m_swapchain = VK_NULL_HANDLE;
        POKE_CORE_INFO("[Vulkan] Destroying Vulkan Swapchain");
    }
}

VkSurfaceFormatKHR VulkanSwapchain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    for (const auto &availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR VulkanSwapchain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
    for (const auto &availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapchain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capablities, Window &window)
{
    if (capablities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capablities.currentExtent;
    }
    else
    {
        int w, h;
        window.GetWindowSize(w, h);

        VkExtent2D actualExtent = {static_cast<uint32_t>(w),
                                   static_cast<uint32_t>(h)};

        actualExtent.width = std::clamp(actualExtent.width, capablities.minImageExtent.width, capablities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capablities.minImageExtent.height, capablities.maxImageExtent.height);

        return actualExtent;
    }
}

void VulkanSwapchain::CreateImageViews(VkDevice logicalDevice)
{
    m_imageViews.resize(m_images.size());

    for (size_t i = 0; i < m_images.size(); ++i)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_images[i];

        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_imageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(logicalDevice, &createInfo, nullptr, &m_imageViews[i]) != VK_SUCCESS)
        {
            POKE_CORE_CRITICAL("Failed to create image views!");
        }
    }

    POKE_CORE_INFO("[Vulkan] Swapchain Image Views created successfully");
}

void VulkanSwapchain::CreateDepthResources(VulkanDevice &device, VulkanPhysicalDevice &physicalDevice)
{
    VkDevice logicalDevice = device.GetHandle();
    VkPhysicalDevice physDevice = physicalDevice.GetHandle();

    m_depthFormat = FindDepthFormat(physDevice);

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = m_extent.width;
    imageInfo.extent.height = m_extent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = m_depthFormat;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(logicalDevice, &imageInfo, nullptr, &m_depthImage) != VK_SUCCESS)
    {
        POKE_CRITICAL("[Vulkan] Failed to create depth image");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(logicalDevice, m_depthImage, &memRequirements);

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physDevice, &memProperties);

    uint32_t memoryTypeIndex = 0;
    bool foundMemory = false;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
    {
        if ((memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
        {
            memoryTypeIndex = i;
            foundMemory = true;
            break;
        }
    }

    if (!foundMemory)
    {
        POKE_CORE_CRITICAL("[Vulkan] Failed to find suitable memory type for depth image");
    }

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &m_depthImageMemory) != VK_SUCCESS)
    {
        POKE_CORE_CRITICAL("[Vulkan] Failed to allocate depth image memory");
    }

    vkBindImageMemory(logicalDevice, m_depthImage, m_depthImageMemory, 0);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_depthImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = m_depthFormat;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(logicalDevice, &viewInfo, nullptr, &m_depthImageView) != VK_SUCCESS)
    {
        POKE_CORE_CRITICAL("[Vulkan] Failed to create depth image view");
    }

    POKE_CORE_INFO("[Vulkan] Depth resources created successfullt");
}

VkFormat VulkanSwapchain::FindDepthFormat(VkPhysicalDevice physicalDevice)
{
    std::vector<VkFormat> candidates = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
    for (VkFormat format : candidates)
    {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &properties);
        if ((properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            return format;
        }
    }
    POKE_CORE_ERROR("[Vulkan] Failed to find supported depth format");
    return VK_FORMAT_UNDEFINED;
}
