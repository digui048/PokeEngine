#include "VertexBuffer.h"

#include "Renderer.h"
#include "Poke/Render/Vulkan/VulkanContext.h"
#include "Poke/Core/Log.h"

using namespace Poke;

VertexBuffer::VertexBuffer(const std::vector<Vertex> &vertices)
    : m_vertexCount(static_cast<uint32_t>(vertices.size()))
{
    VulkanContext &context = Renderer::GetContext();
    VkDevice device = context.GetDevice().GetHandle();

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(vertices[0]) * vertices.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS)
    {
        POKE_CORE_ERROR("Failed to create vertex buffer");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, m_buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &m_bufferMemory) != VK_SUCCESS)
    {
        POKE_CORE_ERROR("Failed to allocate vertex buffer memory");
    }

    vkBindBufferMemory(device, m_buffer, m_bufferMemory, 0);

    void* data;
    vkMapMemory(device, m_bufferMemory, 0, bufferInfo.size, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferInfo.size);
    vkUnmapMemory(device, m_bufferMemory);
}

VertexBuffer::~VertexBuffer()
{
    VulkanContext &context = Renderer::GetContext();
    VkDevice device = context.GetDevice().GetHandle();

    vkDestroyBuffer(device, m_buffer, nullptr);
    vkFreeMemory(device, m_bufferMemory, nullptr);
}

void VertexBuffer::Bind(VkCommandBuffer cmdBuffer) const
{
    VkBuffer vertexBuffers[] = {m_buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
}

uint32_t VertexBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VulkanContext &context = Renderer::GetContext();
    VkPhysicalDevice physicalDevice = context.GetPhysicalDevice().GetHandle();

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    POKE_CORE_ERROR("Failed to find suitable memory type");
    return 0;
}
