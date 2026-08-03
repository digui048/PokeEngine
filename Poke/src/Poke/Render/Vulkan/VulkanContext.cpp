#include "VulkanContext.h"

#include "Poke/Core/Window.h"
#include "Poke/Core/Log.h"

using namespace Poke;

VulkanContext::~VulkanContext()
{
    Shutdown();
}

void VulkanContext::Init(Window &window)
{
    m_instance.Init();
    m_validation.Init(m_instance);
    m_surface.Init(m_instance, window);
    m_physicalDevice.Init(m_instance, m_surface);
    m_device.Init(m_instance, m_physicalDevice);
    m_swapchain.Init(m_device, m_physicalDevice, m_surface, window);
    m_renderPass.Init(m_device, m_swapchain);
    m_framebuffer.Init(m_device, m_swapchain, m_renderPass);
    m_commands.Init(m_device, m_physicalDevice);
    m_sync.Init(m_device, m_swapchain);
}

void VulkanContext::Shutdown()
{
    m_sync.Shutdown(m_device);
    m_commands.Shutdown(m_device);
    m_framebuffer.Shutdown(m_device);
    m_renderPass.Shutdown(m_device);
    m_swapchain.Shutdown(m_device);
    m_device.Shutdown();
    m_physicalDevice.Shutdown();
    m_surface.Shutdown(m_instance);
    m_validation.Shutdown(m_instance);
    m_instance.Shutdown();
}

VkCommandBuffer VulkanContext::BeginFrame(Window &window, float r, float g, float b, float a)
{
    VkDevice device = m_device.GetHandle();
    VkFence inFlightFence = m_sync.GetInFlightFence(m_currentFrame);

    vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);

    VkResult result = vkAcquireNextImageKHR(device, m_swapchain.GetHandle(), UINT64_MAX, m_sync.GetImageAvailableSemaphore(m_currentFrame), VK_NULL_HANDLE, &m_currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapchain(window);
        return VK_NULL_HANDLE;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        POKE_CORE_ERROR("[Vulkan] Failed to aquire swap chain image");
        return VK_NULL_HANDLE;
    }

    vkResetFences(device, 1, &inFlightFence);

    VkCommandBuffer cmd = m_commands.GetCommandBuffer(m_currentFrame);
    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(cmd, &beginInfo);

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_renderPass.GetHandle();
    renderPassInfo.framebuffer = m_framebuffer.GetFramebuffer(m_currentImageIndex);
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_swapchain.GetExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{r, g, b, a}};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_swapchain.GetExtent().width);
    viewport.height = static_cast<float>(m_swapchain.GetExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_swapchain.GetExtent();
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    return cmd;
}

void VulkanContext::EndFrame(Window &window)
{
    VkCommandBuffer cmd = m_commands.GetCommandBuffer(m_currentFrame);
    VkFence inFlightFence = m_sync.GetInFlightFence(m_currentFrame);

    vkCmdEndRenderPass(cmd);

    if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
    {
        POKE_CORE_ERROR("[Vulkan] Failed to record command buffer");
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {m_sync.GetImageAvailableSemaphore(m_currentFrame)};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    VkSemaphore signalSemaphores[] = {m_sync.GetRenderFinishedSemaphore(m_currentImageIndex)};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_device.GetGraphicsQueue(), 1, &submitInfo, inFlightFence) != VK_SUCCESS)
    {
        POKE_CORE_ERROR("[Vulkan] Failed to submit draw command buffer");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapchains[] = {m_swapchain.GetHandle()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &m_currentImageIndex;

    VkResult result = vkQueuePresentKHR(m_device.GetPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized)
    {
        m_framebufferResized = false;
        RecreateSwapchain(window);
    }
    else if (result != VK_SUCCESS)
    {
        POKE_CORE_ERROR("[Vulkan] Failed to present swap chain image");
    }

    m_currentFrame = (m_currentFrame + 1) % VulkanSync::MAX_FRAMES_IN_FLIGHT;
}

VkCommandBuffer VulkanContext::BeginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_commands.GetCommandPool();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_device.GetHandle(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void VulkanContext::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_device.GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_device.GetGraphicsQueue());

    vkFreeCommandBuffers(m_device.GetHandle(), m_commands.GetCommandPool(), 1, &commandBuffer);
}

void VulkanContext::RecreateSwapchain(Window &window)
{
    vkDeviceWaitIdle(m_device.GetHandle());

    m_framebuffer.Shutdown(m_device);
    m_renderPass.Shutdown(m_device);

    VkSwapchainKHR oldSwapchain = m_swapchain.GetHandle();
    m_swapchain.Init(m_device, m_physicalDevice, m_surface, window, oldSwapchain);
    m_renderPass.Init(m_device, m_swapchain);
    m_framebuffer.Init(m_device, m_swapchain, m_renderPass);
}
