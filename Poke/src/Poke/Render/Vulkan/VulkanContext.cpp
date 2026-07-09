#include "VulkanContext.h"

using namespace Poke;

VulkanContext::~VulkanContext()
{
    Shutdown();
}

void VulkanContext::Init()
{
    m_instance.Init();
}

void VulkanContext::Shutdown()
{
    m_instance.Shutdown();
}