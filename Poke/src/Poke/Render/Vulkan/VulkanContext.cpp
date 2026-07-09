#include "VulkanContext.h"

#include "VulkanValidation.h"

using namespace Poke;

VulkanContext::~VulkanContext()
{
    Shutdown();
}

void VulkanContext::Init()
{
    m_instance.Init();
    m_validation.Init(m_instance);
}

void VulkanContext::Shutdown()
{
    m_validation.Shutdown(m_instance);
    m_instance.Shutdown();
}