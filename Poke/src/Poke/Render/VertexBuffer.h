#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include "Vertex.h"
#include "Poke/Render/Vulkan/VulkanBuffer.h"
#include <vector>

namespace Poke
{
    class VertexBuffer : public VulkanBuffer
    {
    public:
        VertexBuffer(const std::vector<Vertex> &vertices);
        ~VertexBuffer() override = default;

        void Bind(VkCommandBuffer cmdBuffer) const;
        uint32_t GetVertexCount() const { return m_vertexCount; }

    private:
        uint32_t m_vertexCount = 0;
    };
}

#endif