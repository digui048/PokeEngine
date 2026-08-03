#include "VulkanFramebuffer.h"

#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanRenderPass.h"
#include "Poke/Core/Log.h"

using namespace Poke;

void VulkanFramebuffer::Init(VulkanDevice &device, VulkanSwapchain &swapchain, VulkanRenderPass &renderPass)
{
    VkDevice logicalDevice = device.GetHandle();
    const auto &imageViews = swapchain.GetImagesViews();
    VkExtent2D extent = swapchain.GetExtent();

    m_framebuffers.resize(imageViews.size());

    for (size_t i = 0; i < imageViews.size(); ++i)
    {
        std::array<VkImageView, 2> attachments = {imageViews[i], swapchain.GetDepthImageView()};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass.GetHandle();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &m_framebuffers[i]) != VK_SUCCESS)
        {
            POKE_CORE_ERROR("[Vulkan] Failed to create framebuffer for index {0}", i);
            return;
        }
    }

    POKE_CORE_INFO("[Vulkan] Created {0} framebuffers successfully", m_framebuffers.size());
}

void VulkanFramebuffer::Shutdown(VulkanDevice &device)
{
    for (auto framebuffer : m_framebuffers)
    {
        vkDestroyFramebuffer(device.GetHandle(), framebuffer, nullptr);
    }

    m_framebuffers.clear();
}
