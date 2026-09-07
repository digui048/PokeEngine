#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#include "Poke/Scene/Component.h"
#include "Poke/Resources/Mesh.h"
#include <memory>

namespace Poke
{
    class MeshComponent : public Component
    {
    public:
        MeshComponent(GameObject *owner, std::shared_ptr<Mesh> mesh = nullptr);
        ~MeshComponent() override = default;

        void SetMesh(std::shared_ptr<Mesh> mesh) { m_mesh = mesh; }
        std::shared_ptr<Mesh> GetMesh() const { return m_mesh; }

        void BindMesh(VkCommandBuffer cmdBuffer) const;

    private:
        std::shared_ptr<Mesh> m_mesh;
    };
}

#endif