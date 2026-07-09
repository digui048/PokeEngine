#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include "VulkanInstance.h"
#include "VulkanValidation.h"

namespace Poke
{
    class VulkanContext
    {
    public:
        VulkanContext() = default;
        ~VulkanContext();
        
        void Init();
        void Shutdown();

        VulkanInstance& GetInstance() { return m_instance; }

    private:
        VulkanInstance m_instance;
        VulkanValidation m_validation;
    };
}

#endif