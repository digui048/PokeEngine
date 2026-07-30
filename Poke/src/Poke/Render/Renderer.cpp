#include "Renderer.h"

#include "Poke/Core/Assert.h"
#include "Poke/Core/Window.h"

#include <SDL3/SDL.h>

#include "Poke/Render/Vulkan/VulkanContext.h"
#include "Poke/Render/UniformBuffer.h"

using namespace Poke;

std::unique_ptr<Poke::VulkanContext> Poke::Renderer::s_Context = nullptr;
Renderer::ClearColor Renderer::s_ClearColor = {0.1f, 0.1f, 0.1f, 1.0f};

void Renderer::Init(Window &window)
{
    s_Context = std::make_unique<VulkanContext>();
    s_Context->Init(window);
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

void Renderer::BindPipeline(VkCommandBuffer cmdBuffer)
{
    s_Context->BindPipeline(cmdBuffer);
}

void Renderer::BindPipelineDescriptors(VkCommandBuffer cmdBuffer, uint32_t currentFrame)
{
    s_Context->BindPipelineDescriptors(cmdBuffer, currentFrame);
}

void Renderer::SetupDescriptors(const UniformBuffer *ubo)
{
    s_Context->SetupDescriptorsPipeline(ubo);
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
