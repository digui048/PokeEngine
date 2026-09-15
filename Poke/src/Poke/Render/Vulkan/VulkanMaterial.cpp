#include "VulkanMaterial.h"
#include "VulkanTexture.h"
#include "VulkanContext.h"
#include "Poke/Render/Renderer.h"
#include "Poke/Core/Log.h"

using namespace Poke;

VulkanMaterial::VulkanMaterial(const VulkanTexture *texture, VkDescriptorPool pool, VkDescriptorSetLayout textureLayout)
{
    if (!texture)
    {
        POKE_CORE_ERROR("[VulkanMaterial] The texture applied is not valid");
        return;
    }

    VkDevice device = Renderer::GetContext().GetDevice().GetHandle();

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &textureLayout;

    if (vkAllocateDescriptorSets(device, &allocInfo, &m_descriptorSet) != VK_SUCCESS)
    {
        POKE_CORE_ERROR("[VulkanMaterial] Failed to allocate material descriptor set");
        return;
    }

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = texture->GetImageView();
    imageInfo.sampler = texture->GetSampler();

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = m_descriptorSet;
    write.dstBinding = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.descriptorCount = 1;
    write.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
}

VulkanMaterial::~VulkanMaterial()
{
}

void VulkanMaterial::Bind(VkCommandBuffer cmdBuffer, VkPipelineLayout pipelineLayout) const
{
    if (m_descriptorSet != VK_NULL_HANDLE)
    {
        vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &m_descriptorSet, 0, nullptr);
    }
}
