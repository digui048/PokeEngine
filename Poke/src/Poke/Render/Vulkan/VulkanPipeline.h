#ifndef VULKAN_PIPELINE_H
#define VULKAN_PIPELINE_H

#include <vulkan/vulkan.h>
#include <string>

namespace Poke
{
    class VulkanDevice;
    class VulkanSwapchain;

    class VulkanPipeline
    {
    public:
        VulkanPipeline() = default;
        ~VulkanPipeline() = default;

        void Init(VulkanDevice& device, VulkanSwapchain& swapchain, const std::string& vertPath, const std::string& fragPath);
        void Shutdown(VulkanDevice& device);

        VkPipeline GetPipeline() const { return m_pipeline; }
        VkPipelineLayout GetPipelineLayout() const { return m_pipelineLayout; }

    private:
        VkPipeline m_pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    };
}

#endif