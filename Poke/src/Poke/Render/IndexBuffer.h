#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H

#include "Poke/Render/Vulkan/VulkanBuffer.h"
#include <vector>

namespace Poke
{
    class IndexBuffer : public VulkanBuffer
    {
    public:
        IndexBuffer(const std::vector<uint16_t> &indices);
        ~IndexBuffer() override = default;

        void Bind(VkCommandBuffer cmdBuffer);
        uint32_t GetIndexCount() const { return m_indexCount; }
        VkIndexType GetIndexType() const { return m_indexType; }

    private:
        uint32_t m_indexCount = 0;
        VkIndexType m_indexType = VK_INDEX_TYPE_UINT16;
    };
}

#endif