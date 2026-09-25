#include "Mesh.h"
#include "Poke/Render/VertexBuffer.h"
#include "Poke/Render/IndexBuffer.h"

using namespace Poke;

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<uint16_t> &indices)
    : m_vertices(vertices), m_indices(indices)
{
    m_vertexBuffer = std::make_unique<VertexBuffer>(m_vertices);
    m_indexBuffer = std::make_unique<IndexBuffer>(m_indices);
}

void Mesh::Bind(VkCommandBuffer cmdBuffer) const
{
    m_vertexBuffer->Bind(cmdBuffer);
    m_indexBuffer->Bind(cmdBuffer);
}
