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
    m_pipeline.Init(m_device, m_swapchain, m_renderPass, "/home/digui048/PokeEngine/build/linux-debug/Poke/assets/shaders/defaultShader.vert.spv", "/home/digui048/PokeEngine/build/linux-debug/Poke/assets/shaders/defaultShader.frag.spv");
    m_framebuffer.Init(m_device, m_swapchain, m_renderPass);
    m_commands.Init(m_device, m_physicalDevice);
    m_sync.Init(m_device);
}

void VulkanContext::Shutdown()
{
    m_sync.Shutdown(m_device);
    m_commands.Shutdown(m_device);
    m_framebuffer.Shutdown(m_device);
    m_pipeline.Shutdown(m_device);
    m_renderPass.Shutdown(m_device);
    m_swapchain.Shutdown(m_device);
    m_device.Shutdown();
    m_physicalDevice.Shutdown();
    m_surface.Shutdown(m_instance);
    m_validation.Shutdown(m_instance);
    m_instance.Shutdown();
}

VkCommandBuffer VulkanContext::BeginFrame(float r, float g, float b, float a)
{
    VkDevice device = m_device.GetHandle();
    VkFence inFlightFence = m_sync.GetInFlightFence();
    VkCommandBuffer cmd = m_commands.GetCommandBuffer();

    vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &inFlightFence);
    vkAcquireNextImageKHR(device, m_swapchain.GetHandle(), UINT64_MAX, m_sync.GetImageAvailableSemaphore(), VK_NULL_HANDLE, &m_currentImageIndex);
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

    VkClearValue clearColor = {{{r, g, b, a}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    return cmd;
}

void VulkanContext::EndFrame()
{
    VkCommandBuffer cmd = m_commands.GetCommandBuffer();
    VkFence inFlightFence = m_sync.GetInFlightFence();

    vkCmdEndRenderPass(cmd);

    if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
    {
        POKE_CORE_ERROR("[Vulkan] Failed to record command buffer");
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {m_sync.GetImageAvailableSemaphore()};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    VkSemaphore signalSemaphores[] = {m_sync.GetRenderFinishedSemaphore()};
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

    vkQueuePresentKHR(m_device.GetPresentQueue(), &presentInfo);
}

void VulkanContext::DrawTriangle()
{
    VkCommandBuffer cmd = m_commands.GetCommandBuffer();
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.GetPipeline());

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

    vkCmdDraw(cmd, 3, 1, 0, 0);
}
