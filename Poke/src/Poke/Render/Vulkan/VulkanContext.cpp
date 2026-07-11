#include "VulkanContext.h"

using namespace Poke;

VulkanContext::~VulkanContext()
{
    Shutdown();
}

void VulkanContext::Init()
{
    m_instance.Init();
    m_validation.Init(m_instance);
    m_physicalDevice.Init(m_instance);
    m_device.Init(m_instance, m_physicalDevice);
}

void VulkanContext::Shutdown()
{
    m_device.Shutdown();
    m_physicalDevice.Shutdown();
    m_validation.Shutdown(m_instance);
    m_instance.Shutdown();
}