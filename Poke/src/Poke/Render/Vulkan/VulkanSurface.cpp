#include "VulkanSurface.h"

#include "Poke/Core/Window.h"
#include "Poke/Core/Log.h"

#include "VulkanInstance.h"

#include <SDL3/SDL_vulkan.h>

using namespace Poke;

VulkanSurface::~VulkanSurface()
{
}

void VulkanSurface::Init(VulkanInstance &instance, Window &window)
{
    if (!SDL_Vulkan_CreateSurface(window.GetSDLWindow(), instance.GetHandle(), nullptr, &m_surface))
    {
        POKE_CORE_CRITICAL("Failed to create vulkan surface: {0}", SDL_GetError());
    }
}

void VulkanSurface::Shutdown(VulkanInstance &instance)
{
    if (m_surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(instance.GetHandle(), m_surface, nullptr);
        m_surface = VK_NULL_HANDLE;
        POKE_CORE_INFO("[Vulkan] Destroying Vulkan Surface");
    }
}
