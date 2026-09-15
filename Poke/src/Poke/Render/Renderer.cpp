#include "Renderer.h"

#include "Poke/Core/Assert.h"
#include "Poke/Core/Window.h"

#include <SDL3/SDL.h>

#include <IL/il.h>

#include "Poke/Render/Vulkan/VulkanContext.h"
#include "Poke/Render/Vulkan/VulkanPipeline.h"
#include "Poke/Render/UniformBuffer.h"
#include "Poke/Resources/Mesh.h"
#include "Poke/Resources/Material.h"

using namespace Poke;

std::unique_ptr<Poke::VulkanContext> Poke::Renderer::s_Context = nullptr;
std::unique_ptr<UniformBuffer> Renderer::s_defaultUniformBuffer = nullptr;
Renderer::ClearColor Renderer::s_ClearColor = {0.1f, 0.1f, 0.1f, 1.0f};
std::vector<RenderItem> Renderer::s_renderQueue = {};

void Renderer::Init(Window &window)
{
    s_Context = std::make_unique<VulkanContext>();
    s_Context->Init(window);

    s_defaultUniformBuffer = std::make_unique<Poke::UniformBuffer>(sizeof(CameraData));

    ilInit();
}

void Renderer::Shutdown()
{
    s_defaultUniformBuffer.reset();
    s_Context.reset();
}

void Renderer::WaitIdle()
{
    if (s_Context)
    {
        vkDeviceWaitIdle(s_Context->GetDevice().GetHandle());
    }
}

VkCommandBuffer Renderer::BeginFrame(Window &window)
{
    return s_Context->BeginFrame(window, s_ClearColor.r, s_ClearColor.g, s_ClearColor.b, s_ClearColor.a);
}

void Renderer::EndFrame(Window &window)
{
    s_Context->EndFrame(window);
}

std::shared_ptr<VulkanPipeline> Renderer::CreatePipeline(const std::string &vertPath, const std::string &fragPath, const std::vector<VkPushConstantRange> &pushConstantRanges)
{
    auto pipeline = std::make_shared<VulkanPipeline>();
    pipeline->Init(s_Context->GetDevice(), s_Context->GetSwapchain(), s_Context->GetRenderPass(), vertPath, fragPath, pushConstantRanges);
    return pipeline;
}

void Renderer::BindPipeline(VkCommandBuffer cmdBuffer, const std::shared_ptr<VulkanPipeline> &pipeline)
{
    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());
}

void Renderer::BindPipelineDescriptors(VkCommandBuffer cmdBuffer, const std::shared_ptr<VulkanPipeline> &pipeline)
{
    pipeline->BindGlobalDescriptors(cmdBuffer, s_Context->GetCurrentFrame());
}

void Renderer::FrameResized()
{
    s_Context->FlagFramebufferResized();
}

void Renderer::SetClearColor(const float r, const float g, const float b, const float a)
{
    s_ClearColor = {r, g, b, a};
}

void Renderer::UpdateCameraBuffer(const glm::mat4 &view, const glm::mat4 &projection)
{
    CameraData cam{view, projection};
    s_defaultUniformBuffer->SetData(&cam);
}

void Renderer::SubmitRenderItem(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, const glm::mat4 &transform)
{
    if (!mesh)
        return;
    s_renderQueue.push_back({mesh, material, transform});
}

void Renderer::FlushQueue(VkCommandBuffer cmdBuffer, const std::shared_ptr<VulkanPipeline> &pipeline)
{
    BindPipeline(cmdBuffer, pipeline);
    BindPipelineDescriptors(cmdBuffer, pipeline);

    for (const auto &item : s_renderQueue)
    {
        pipeline->PushConstants(cmdBuffer, VK_SHADER_STAGE_VERTEX_BIT, item.transform);
        if (item.material && item.material->GetVulkanMaterial())
        {
            item.material->Bind(cmdBuffer, pipeline->GetPipelineLayout());
        }
        item.mesh->Bind(cmdBuffer);
        vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(item.mesh->GetIndices().size()), 1, 0, 0, 0);
    }

    s_renderQueue.clear();
}

uint32_t Renderer::GetCurrentFrame()
{
    return GetContext().GetCurrentFrame();
}
