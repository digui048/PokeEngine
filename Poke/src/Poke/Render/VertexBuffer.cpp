#include "VertexBuffer.h"

#include "Renderer.h"
#include "Poke/Render/Vulkan/VulkanContext.h"
#include "Poke/Core/Log.h"

using namespace Poke;

VertexBuffer::VertexBuffer(const std::vector<Vertex> &vertices)
    : VulkanBuffer(vertices.data(), sizeof(vertices[0]) * vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT),
    m_vertexCount(static_cast<uint32_t>(vertices.size()))
{
}

void VertexBuffer::Bind(VkCommandBuffer cmdBuffer) const
{
    VkBuffer vertexBuffers[] = {m_buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
}