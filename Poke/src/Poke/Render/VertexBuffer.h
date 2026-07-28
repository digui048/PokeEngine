#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include "Vertex.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Poke
{
    class VertexBuffer
    {
    public:
        VertexBuffer(const std::vector<Vertex>& vertices);
        ~VertexBuffer();

        void Bind(VkCommandBuffer cmdBuffer) const;

        uint32_t GetVertexCount() const { return m_vertexCount; }

    private:
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    private:
        VkBuffer m_buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_bufferMemory = VK_NULL_HANDLE;
        uint32_t m_vertexCount = 0;
    };
}

#endif