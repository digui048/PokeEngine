#include "UniformBuffer.h"

#include "Renderer.h"
#include "Poke/Render/Vulkan/VulkanContext.h"
#include "Poke/Render/Vulkan/VulkanSync.h"
#include "Poke/Core/Log.h"

using namespace Poke;

UniformBuffer::UniformBuffer(uint32_t size)
    : m_size(size)
{
    VulkanContext& context = Renderer::GetContext();
    VkDevice device = context.GetDevice().GetHandle();

    m_buffers.resize(VulkanSync::MAX_FRAMES_IN_FLIGHT);
    m_memory.resize(VulkanSync::MAX_FRAMES_IN_FLIGHT);
    m_mapped.resize(VulkanSync::MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < VulkanSync::MAX_FRAMES_IN_FLIGHT; ++i)
    {
        CreateBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_buffers[i], m_memory[i]);
        vkMapMemory(device, m_memory[i], 0, size, 0, &m_mapped[i]);
    }
}

UniformBuffer::~UniformBuffer()
{
    VkDevice device = Renderer::GetContext().GetDevice().GetHandle();

    for(size_t i = 0; i < VulkanSync::MAX_FRAMES_IN_FLIGHT; ++i)
    {
        vkDestroyBuffer(device, m_buffers[i], nullptr);
        vkFreeMemory(device, m_memory[i], nullptr);
    }
}

void UniformBuffer::SetData(const void *data)
{
    uint32_t currentFrame = Renderer::GetCurrentFrame();

    mempcpy(m_mapped[currentFrame], data, m_size);
}

uint32_t UniformBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
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

void UniformBuffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory)
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
