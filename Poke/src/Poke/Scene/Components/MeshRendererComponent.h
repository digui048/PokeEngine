#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#include "Poke/Scene/Component.h"
#include "Poke/Resources/Assets/Mesh.h"
#include "Poke/Resources/Assets/Material.h"
#include <memory>

namespace Poke
{
    class MeshRendererComponent : public Component
    {
    public:
        MeshRendererComponent(GameObject *owner, std::shared_ptr<Mesh> mesh = nullptr, std::shared_ptr<Material> material = nullptr);
        ~MeshRendererComponent() override = default;

        void SetMesh(std::shared_ptr<Mesh> mesh) { m_mesh = mesh; }
        std::shared_ptr<Mesh> GetMesh() const { return m_mesh; }

        void SetMaterial(std::shared_ptr<Material> material) { m_material = material; }
        std::shared_ptr<Material> GetMaterial() const { return m_material; }

        size_t GetVerticesCount() const { return m_mesh ? m_mesh->GetVerticesCount() : 0; }
        size_t GetIndicesCount() const { return m_mesh ? m_mesh->GetIndicesCount() : 0; }

        void BindMesh(VkCommandBuffer cmdBuffer) const;

    private:
        std::shared_ptr<Mesh> m_mesh;
        std::shared_ptr<Material> m_material;
    };
}

#endif