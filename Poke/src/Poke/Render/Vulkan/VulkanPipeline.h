#ifndef VULKAN_PIPELINE_H
#define VULKAN_PIPELINE_H

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

#include "Poke/Render/UniformBuffer.h"
#include "Poke/Render/Vulkan/VulkanTexture.h"

namespace Poke
{
    class VulkanDevice;
    class VulkanSwapchain;
    class VulkanRenderPass;

    class VulkanPipeline
    {
    public:
        VulkanPipeline() = default;
        ~VulkanPipeline();

        void Init(VulkanDevice &device, VulkanSwapchain &swapchain, VulkanRenderPass &renderPass, const std::string &vertPath, const std::string &fragPath, const std::vector<VkPushConstantRange>& pushConstantRanges);
        void Shutdown(VulkanDevice &device);

        VkPipeline GetPipeline() const { return m_pipeline; }
        VkPipelineLayout GetPipelineLayout() const { return m_pipelineLayout; }

        void SetupDescriptors(const UniformBuffer *uniformBuffer, const VulkanTexture *textureBuffer);
        void BindDescriptors(VkCommandBuffer cmd, uint32_t currentFrame);

        template<typename T>
        void PushConstants(VkCommandBuffer cmd, VkShaderStageFlags stageFlags, const T& data, uint32_t offset = 0);

    private:
        VkPipeline m_pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
        std::vector<VkDescriptorSet> m_descriptorSets;
    };
}

#endif