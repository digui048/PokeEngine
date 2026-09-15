#ifndef MATERIAL_IMPORTER_H
#define MATERIAL_IMPORTER_H

#include "Poke/Resources/Material.h"
#include <string>
#include <memory>

namespace Poke
{
    class VulkanPipeline;
    class Texture;

    class MaterialImporter
    {
    public:
        MaterialImporter() = delete;

        static std::shared_ptr<Material> LoadMaterial(std::shared_ptr<Texture> texture, const VulkanPipeline *pipeline);
    };
}

#endif