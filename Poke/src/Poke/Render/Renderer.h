#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace Poke
{
    class VulkanContext;
    class Window;
    class VulkanPipeline;
    class UniformBuffer;
    class Mesh;

    struct RenderItem
    {
        std::shared_ptr<Mesh> mesh;
        glm::mat4 transform;
    };

    class Renderer
    {
    public:
        static void Init(Window &window);
        static void Shutdown();
        static void WaitIdle();

        static VkCommandBuffer BeginFrame(Window &window);
        static void EndFrame(Window &window);

        static std::shared_ptr<VulkanPipeline> CreatePipeline(const std::string &vertPath, const std::string &fragPath, const std::vector<VkPushConstantRange>& pushConstantRanges);
        static void BindPipeline(VkCommandBuffer cmdBuffer, const std::shared_ptr<VulkanPipeline> &pipeline);
        static void BindPipelineDescriptors(VkCommandBuffer cmdBuffer, const std::shared_ptr<VulkanPipeline> &pipeline);

        static void FrameResized();

        static void SetClearColor(const float r, const float g, const float b, const float a);

        static void UpdateCameraBuffer(const glm::mat4 &view, const glm::mat4 &projection);
        static void SubmitRenderItem(std::shared_ptr<Mesh> mesh, const glm::mat4 &transform);
        static void FlushQueue(VkCommandBuffer cmdBuffer, const std::shared_ptr<VulkanPipeline> &pipeline);

        static VulkanContext &GetContext() { return *s_Context; }
        static uint32_t GetCurrentFrame();
        static UniformBuffer *GetDefaultUniformBuffer() { return s_defaultUniformBuffer.get(); }

    private:
        struct ClearColor
        {
            float r, g, b, a;
        };

    private:
        static std::unique_ptr<VulkanContext> s_Context;
        static ClearColor s_ClearColor;
        static std::unique_ptr<UniformBuffer> s_defaultUniformBuffer;
        static std::vector<RenderItem> s_renderQueue;
    };
}

#endif