#include "VulkanContext.h"

#include "Poke/Core/Window.h"

using namespace Poke;

VulkanContext::~VulkanContext()
{
    Shutdown();
}

void VulkanContext::Init(Window& window)
{
    m_instance.Init();
    m_validation.Init(m_instance);
    m_surface.Init(m_instance, window);
    m_physicalDevice.Init(m_instance, m_surface);
    m_device.Init(m_instance, m_physicalDevice);
    m_swapchain.Init(m_device, m_physicalDevice, m_surface, window);
}

void VulkanContext::Shutdown()
{
    m_swapchain.Shutdown(m_device);
    m_device.Shutdown();
    m_physicalDevice.Shutdown();
    m_surface.Shutdown(m_instance);
    m_validation.Shutdown(m_instance);
    m_instance.Shutdown();
}