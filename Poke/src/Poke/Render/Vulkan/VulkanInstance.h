#ifndef VULKAN_INSTANCE_H
#define VULKAN_INSTANCE_H

#include <vulkan/vulkan.h>

#include <vector>

namespace Poke
{
    class VulkanValidation;

    class VulkanInstance
    {
    public:
        VulkanInstance() = default;
        ~VulkanInstance();

        void Init();
        void Shutdown();

        VkInstance GetHandle() const { return m_instance; }

        std::vector<const char*> GetRequiredExtensions() const;

    private:
        void CreateInstance();

    private:
        VkInstance m_instance = VK_NULL_HANDLE;
    };
}

#endif