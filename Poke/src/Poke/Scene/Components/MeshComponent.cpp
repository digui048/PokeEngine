#include "MeshComponent.h"

using namespace Poke;

MeshComponent::MeshComponent(GameObject *owner, std::shared_ptr<Mesh> mesh)
    : Component(owner, ComponentType::MESH), m_mesh(mesh) {}

void MeshComponent::BindMesh(VkCommandBuffer cmdBuffer) const
{
    if (m_mesh)
    {
        m_mesh->Bind(cmdBuffer);
    }
}
