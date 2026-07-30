#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <string>
#include <vulkan/vulkan.h>

namespace Poke
{
    class VulkanContext;
    class Window;
    class VulkanPipeline;

    class Renderer
    {
    public:
        static void Init(Window &window);
        static void Shutdown();
        static void WaitIdle();

        static VkCommandBuffer BeginFrame(Window &window);
        static void EndFrame(Window &window);
        
        static std::shared_ptr<VulkanPipeline> CreatePipeline(const std::string& vertPath, const std::string& fragPath);

        static void BindPipeline(VkCommandBuffer cmdBuffer, const std::shared_ptr<VulkanPipeline>& pipeline);
        static void BindPipelineDescriptors(VkCommandBuffer cmdBuffer, const std::shared_ptr<VulkanPipeline>& pipeline);

        static void FrameResized();

        static void SetClearColor(const float r, const float g, const float b, const float a);

        static VulkanContext& GetContext() { return *s_Context; }
        static uint32_t GetCurrentFrame();

    private:
        static std::unique_ptr<VulkanContext> s_Context;
        static struct ClearColor
        {
            float r, g, b, a;
        } s_ClearColor;
    };
}

#endif