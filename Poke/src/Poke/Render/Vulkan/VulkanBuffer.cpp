#include "VulkanBuffer.h"

#include "Poke/Render/Renderer.h"
#include "Poke/Render/Vulkan/VulkanContext.h"
#include "Poke/Core/Log.h"

using namespace Poke;

VulkanBuffer::VulkanBuffer(const void *data, VkDeviceSize size, VkBufferUsageFlags usage)
    : m_size(size)
{
    VulkanContext &context = Renderer::GetContext();
    VkDevice device = context.GetDevice().GetHandle();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void *mappedData;
    vkMapMemory(device, stagingBufferMemory, 0, size, 0, &mappedData);
    memcpy(mappedData, data, static_cast<size_t>(size));
    vkUnmapMemory(device, stagingBufferMemory);

    CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_buffer, m_bufferMemory);
    CopyBuffer(stagingBuffer, m_buffer, size);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

VulkanBuffer::~VulkanBuffer()
{
    VulkanContext &context = Renderer::GetContext();
    VkDevice device = context.GetDevice().GetHandle();

    if (m_buffer != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(device, m_buffer, nullptr);
    }
    if (m_bufferMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(device, m_bufferMemory, nullptr);
    }
}

uint32_t VulkanBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
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

void VulkanBuffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory)
{
    VulkanContext &context = Renderer::GetContext();
    VkDevice device = context.GetDevice().GetHandle();

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    {
        POKE_CORE_ERROR("Failed to create buffer");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
    {
        POKE_CORE_ERROR("Failed to allocate buffer memory");
    }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void VulkanBuffer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VulkanContext &context = Renderer::GetContext();

    VkCommandBuffer commandBuffer = context.BeginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    context.EndSingleTimeCommands(commandBuffer);
}
