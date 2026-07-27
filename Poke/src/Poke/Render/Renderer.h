#ifndef RENDERER_H
#define RENDERER_H

#include <memory>

namespace Poke
{
    class VulkanContext;
    class Window;

    class Renderer
    {
    public:
        static void Init(Window& window);
        static void Shutdown();
        static void WaitIdle();

        static bool BeginFrame(Window& window);
        static void EndFrame(Window& window);
        static void DrawTriangle();

        static void FrameResized();

        static void SetClearColor(const float r, const float g, const float b, const float a);
    private:
        static std::unique_ptr<VulkanContext> s_Context;
        static struct ClearColor { float r, g, b, a; } s_ClearColor;
    };
}

#endif