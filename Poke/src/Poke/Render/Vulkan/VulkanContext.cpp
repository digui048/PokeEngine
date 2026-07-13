#include "VulkanContext.h"

#include "Poke/Core/Application.h"
#include "Poke/Core/Window.h"

using namespace Poke;

VulkanContext::~VulkanContext()
{
    Shutdown();
}

void VulkanContext::Init()
{
    m_instance.Init();
    m_validation.Init(m_instance);
    m_surface.Init(m_instance, *Application::GetInstance().GetWindow());
    m_physicalDevice.Init(m_instance, m_surface);
    m_device.Init(m_instance, m_physicalDevice);
}

void VulkanContext::Shutdown()
{
    m_device.Shutdown();
    m_physicalDevice.Shutdown();
    m_surface.Shutdown(m_instance);
    m_validation.Shutdown(m_instance);
    m_instance.Shutdown();
}