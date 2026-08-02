#include "Renderer.h"

#include "Poke/Core/Assert.h"
#include "Poke/Core/Window.h"

#include <SDL3/SDL.h>

#include <IL/il.h>

#include "Poke/Render/Vulkan/VulkanContext.h"
#include "Poke/Render/Vulkan/VulkanPipeline.h"

using namespace Poke;

std::unique_ptr<Poke::VulkanContext> Poke::Renderer::s_Context = nullptr;
Renderer::ClearColor Renderer::s_ClearColor = {0.1f, 0.1f, 0.1f, 1.0f};

void Renderer::Init(Window &window)
{
    s_Context = std::make_unique<VulkanContext>();
    s_Context->Init(window);
    ilInit();
}

void Renderer::Shutdown()
{
    s_Context.reset();
}

void Renderer::WaitIdle()
{
    if (s_Context)
    {
        vkDeviceWaitIdle(s_Context->GetDevice().GetHandle());
    }
}

VkCommandBuffer Renderer::BeginFrame(Window& window)
{
    return s_Context->BeginFrame(window, s_ClearColor.r, s_ClearColor.g, s_ClearColor.b, s_ClearColor.a);
}

void Renderer::EndFrame(Window& window)
{
    s_Context->EndFrame(window);
}

std::shared_ptr<VulkanPipeline> Renderer::CreatePipeline(const std::string &vertPath, const std::string &fragPath)
{
    auto pipeline = std::make_shared<VulkanPipeline>();
    pipeline->Init(s_Context->GetDevice(), s_Context->GetSwapchain(), s_Context->GetRenderPass(), vertPath, fragPath);
    return pipeline;
}

void Renderer::BindPipeline(VkCommandBuffer cmdBuffer, const std::shared_ptr<VulkanPipeline>& pipeline)
{
    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());
}

void Renderer::BindPipelineDescriptors(VkCommandBuffer cmdBuffer, const std::shared_ptr<VulkanPipeline>& pipeline)
{
    pipeline->BindDescriptors(cmdBuffer, s_Context->GetCurrentFrame());
}

void Renderer::FrameResized()
{
    s_Context->FlagFramebufferResized();
}

void Renderer::SetClearColor(const float r, const float g, const float b, const float a)
{
    s_ClearColor = {r, g, b, a};
}

uint32_t Renderer::GetCurrentFrame()
{
    return GetContext().GetCurrentFrame();
}
