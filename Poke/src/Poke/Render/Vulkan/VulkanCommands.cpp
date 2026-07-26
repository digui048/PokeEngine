#include "VulkanCommands.h"

#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanPipeline.h"
#include "Poke/Core/Log.h"

using namespace Poke;

void VulkanCommands::Init(VulkanDevice &device, VulkanPhysicalDevice &physicalDevice)
{
    VkDevice logicalDevice = device.GetHandle();
    QueueFamilyIndices queueFamilyIndices = physicalDevice.GetQueueFamilies();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();

    if (vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
    {
        POKE_CORE_ERROR("[Vulkan] Failed to create command pool!");
        return;
    }

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, &m_commandBuffer) != VK_SUCCESS)
    {
        POKE_CORE_ERROR("[Vulkan] Failed to allocate command buffer!");
        return;
    }

    POKE_CORE_INFO("[Vulkan] Command Pool and Buffer created successfully");
}

void VulkanCommands::Shutdown(VulkanDevice &device)
{
    if (m_commandPool != VK_NULL_HANDLE)
    {
        vkDestroyCommandPool(device.GetHandle(), m_commandPool, nullptr);
        m_commandPool = VK_NULL_HANDLE;
    }
}

void VulkanCommands::RecordCommandBuffer(VulkanSwapchain &swapchain, VulkanRenderPass &renderPass, VulkanFramebuffer &framebuffers, VulkanPipeline &pipeline, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(m_commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        POKE_CORE_ERROR("[Vulkan] Failed to begin recording comand buffers");
        return;
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass.GetHandle();
    renderPassInfo.framebuffer = framebuffers.GetFramebuffer(imageIndex);
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapchain.GetExtent();

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(m_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetPipeline());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapchain.GetExtent().width);
    viewport.height = static_cast<float>(swapchain.GetExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapchain.GetExtent();
    vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);

    vkCmdDraw(m_commandBuffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(m_commandBuffer);

    if (vkEndCommandBuffer(m_commandBuffer) != VK_SUCCESS)
    {
        POKE_CORE_ERROR("[Vulkan] Failed to record command buffer");
    }
}
