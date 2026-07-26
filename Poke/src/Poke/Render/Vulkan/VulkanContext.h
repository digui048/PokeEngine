#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include "VulkanInstance.h"
#include "VulkanValidation.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanSurface.h"
#include "VulkanSwapchain.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanCommands.h"
#include "VulkanSync.h"

namespace Poke
{
    class Window;

    class VulkanContext
    {
    public:
        VulkanContext() = default;
        ~VulkanContext();

        void Init(Window &window);
        void Shutdown();

        VkCommandBuffer BeginFrame(float r, float g, float b, float a);
        void EndFrame();

        void DrawTriangle();

        VulkanInstance &GetInstance() { return m_instance; }
        VulkanDevice &GetDevice() { return m_device; }

    private:
        VulkanInstance m_instance;
        VulkanValidation m_validation;
        VulkanPhysicalDevice m_physicalDevice;
        VulkanDevice m_device;
        VulkanSurface m_surface;
        VulkanSwapchain m_swapchain;
        VulkanRenderPass m_renderPass;
        VulkanPipeline m_pipeline;
        VulkanFramebuffer m_framebuffer;
        VulkanCommands m_commands;
        VulkanSync m_sync;
    private:
        uint32_t m_currentImageIndex = 0;
        uint32_t m_currentFrame = 0;
    };
}

#endif