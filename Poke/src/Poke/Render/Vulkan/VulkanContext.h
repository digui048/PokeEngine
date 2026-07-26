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

namespace Poke
{
    class Window;

    class VulkanContext
    {
    public:
        VulkanContext() = default;
        ~VulkanContext();
        
        void Init(Window& window);
        void Shutdown();

        VulkanInstance& GetInstance() { return m_instance; }

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
    };
}

#endif