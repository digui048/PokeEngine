#ifndef MESH_H
#define MESH_H

#include "Poke/Render/VertexBuffer.h"
#include "Poke/Render/IndexBuffer.h"
#include "Poke/Render/Vertex.h"

#include <vector>
#include <memory>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace Poke
{
    class Mesh
    {
    public:
        Mesh(const std::vector<Vertex> &vertices, const std::vector<uint16_t> &indices);
        ~Mesh() = default;

        void Bind(VkCommandBuffer cmdBuffer) const;

        const std::vector<Vertex> &GetVertices() const { return m_vertices; }
        const std::vector<uint16_t> &GetIndices() const { return m_indices; }
        
        size_t GetVerticesCount() const { return m_vertices.size(); }
        size_t GetIndicesCount() const { return m_indices.size(); }

    private:
        std::vector<Vertex> m_vertices;
        std::vector<uint16_t> m_indices;

        std::unique_ptr<VertexBuffer> m_vertexBuffer;
        std::unique_ptr<IndexBuffer> m_indexBuffer;
    };
}

#endif