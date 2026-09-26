#ifndef MATERIAL_H
#define MATERIAL_H

#include "Poke/Resources/Asset.h"

#include <memory>
#include <vulkan/vulkan.h>

namespace Poke
{
    class Texture;
    class VulkanMaterial;
    class VulkanPipeline;

    class Material : public Asset
    {
    public:
        Material(std::shared_ptr<Texture> texture, const VulkanPipeline *pipeline);
        Material(AssetHandle handle, std::shared_ptr<Texture> texture, const VulkanPipeline *pipeline);
        ~Material();

        void SetTexture(std::shared_ptr<Texture> texture, const VulkanPipeline *pipeline);
        std::shared_ptr<Texture> GetTexture() const { return m_texture; }

        AssetType GetType() const override { return AssetType::Material; }

        void Bind(VkCommandBuffer cmdBuffer, VkPipelineLayout pipelineLayout);
        const VulkanMaterial *GetVulkanMaterial() const { return m_vulkanMaterial.get(); }

    private:
        std::shared_ptr<Texture> m_texture;
        std::unique_ptr<VulkanMaterial> m_vulkanMaterial;
    };
}

#endif