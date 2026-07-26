#include "VulkanContext.h"

#include "Poke/Core/Window.h"

using namespace Poke;

VulkanContext::~VulkanContext()
{
    Shutdown();
}

void VulkanContext::Init(Window &window)
{
    m_instance.Init();
    m_validation.Init(m_instance);
    m_surface.Init(m_instance, window);
    m_physicalDevice.Init(m_instance, m_surface);
    m_device.Init(m_instance, m_physicalDevice);
    m_swapchain.Init(m_device, m_physicalDevice, m_surface, window);
    m_renderPass.Init(m_device, m_swapchain);
    m_pipeline.Init(m_device, m_swapchain, m_renderPass, "/home/digui048/PokeEngine/build/linux-debug/Poke/assets/shaders/defaultShader.vert.spv", "/home/digui048/PokeEngine/build/linux-debug/Poke/assets/shaders/defaultShader.frag.spv");
    m_framebuffer.Init(m_device, m_swapchain, m_renderPass);
}

void VulkanContext::Shutdown()
{
    m_framebuffer.Shutdown(m_device);
    m_pipeline.Shutdown(m_device);
    m_renderPass.Shutdown(m_device);
    m_swapchain.Shutdown(m_device);
    m_device.Shutdown();
    m_physicalDevice.Shutdown();
    m_surface.Shutdown(m_instance);
    m_validation.Shutdown(m_instance);
    m_instance.Shutdown();
}