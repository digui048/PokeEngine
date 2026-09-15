#include "Material.h"
#include "Texture.h"

#include "Poke/Render/Vulkan/VulkanMaterial.h"
#include "Poke/Render/Vulkan/VulkanPipeline.h"

using namespace Poke;

Material::Material(std::shared_ptr<Texture> texture, const VulkanPipeline *pipeline)
{
    SetTexture(texture, pipeline);
}

Material::~Material()
{
    m_vulkanMaterial.reset();
}

void Material::SetTexture(std::shared_ptr<Texture> texture, const VulkanPipeline *pipeline)
{
    m_texture = texture;

    if (m_texture && pipeline && m_texture->GetVulkanTexture())
    {
        m_vulkanMaterial = std::make_unique<VulkanMaterial>(m_texture->GetVulkanTexture(), pipeline->GetDescriptorPool(), pipeline->GetTextureDescriptorSetLayout());
    }
}

void Material::Bind(VkCommandBuffer cmdBuffer, VkPipelineLayout pipelineLayout)
{
    m_vulkanMaterial->Bind(cmdBuffer, pipelineLayout);
}
