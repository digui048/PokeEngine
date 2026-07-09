#ifndef VULKAN_INSTANCE_H
#define VULKAN_INSTANCE_H

#include <vulkan/vulkan.h>

namespace Poke
{
    class VulkanInstance
    {
    public:
        VulkanInstance() = default;
        ~VulkanInstance();

        void Init();
        void Shutdown();

        VkInstance GetInstance() const { return m_instance; }

    private:
        void CreateInstance();

    private:
        VkInstance m_instance = VK_NULL_HANDLE;
    };
}

#endif