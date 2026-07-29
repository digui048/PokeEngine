#include "IndexBuffer.h"

using namespace Poke;

IndexBuffer::IndexBuffer(const std::vector<uint16_t> &indices)
    : VulkanBuffer(indices.data(), sizeof(indices[0]) * indices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT),
    m_indexCount(static_cast<uint32_t>(indices.size())),
    m_indexType(VK_INDEX_TYPE_UINT16)
{
}

void IndexBuffer::Bind(VkCommandBuffer cmdBuffer)
{
    vkCmdBindIndexBuffer(cmdBuffer, m_buffer, 0, m_indexType);
}
