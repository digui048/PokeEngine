#include "MeshRendererComponent.h"

using namespace Poke;

MeshRendererComponent::MeshRendererComponent(GameObject *owner, std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
    : Component(owner, ComponentType::MESH), m_mesh(mesh), m_material(material) {}

void MeshRendererComponent::BindMesh(VkCommandBuffer cmdBuffer) const
{
    if (m_mesh)
    {
        m_mesh->Bind(cmdBuffer);
    }
}
