#ifndef VULKAN_MATERIAL_H
#define VULKAN_MATERIAL_H

#include <vulkan/vulkan.h>

namespace Poke
{
    class VulkanTexture;

    class VulkanMaterial
    {
    public:
        VulkanMaterial(const VulkanTexture *texture, VkDescriptorPool pool, VkDescriptorSetLayout textureLayout);
        ~VulkanMaterial();

        void Bind(VkCommandBuffer cmdBuffer, VkPipelineLayout pipelineLayout) const;
        VkDescriptorSet GetDescriptorSet() const { return m_descriptorSet; }

    private:
        VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
    };
}

#endif