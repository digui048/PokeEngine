#include "MaterialImporter.h"
#include "Poke/Core/Log.h"

using namespace Poke;

std::shared_ptr<Material> MaterialImporter::LoadMaterial(std::shared_ptr<Texture> texture, const VulkanPipeline *pipeline)
{
    if (!texture)
    {
        POKE_CORE_ERROR("[MaterialImporter] The texture is invalid");
        return nullptr;
    }

    if (!pipeline)
    {
        POKE_CORE_ERROR("[MaterialImporter] The pipeline is invalid");
        return nullptr;
    }

    return std::make_shared<Material>(texture, pipeline);
}