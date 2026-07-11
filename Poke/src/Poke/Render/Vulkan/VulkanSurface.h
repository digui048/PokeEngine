#ifndef VULKAN_SURFACE_H
#define VULKAN_SURFACE_H

#include <vulkan/vulkan.h>

namespace Poke
{
    class VulkanInstance;
    class Window;

    class VulkanSurface
    {
    public:
        VulkanSurface() = default;
        ~VulkanSurface();

        void Init(VulkanInstance& instance, Window& window);
        void Shutdown(VulkanInstance& instance);

        VkSurfaceKHR GetHandle() const { return m_surface; }

    private:
        VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    };
}

#endif