#include "Renderer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#include "Poke/Core/Assert.h"
#include "Poke/Core/Window.h"

#include <glad/glad.h>
#include <SDL3/SDL.h>

#include "Poke/Render/Vulkan/VulkanContext.h"

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

void Renderer::BeginFrame()
{
    s_Context->BeginFrame(s_ClearColor.r, s_ClearColor.g, s_ClearColor.b, s_ClearColor.a);
}

void Renderer::EndFrame()
{
    s_Context->EndFrame();
}

void Renderer::DrawTriangle()
{
    s_Context->DrawTriangle();
}

void Renderer::SetClearColor(const float r, const float g, const float b, const float a)
{
    s_ClearColor = {r, g, b, a};
}
