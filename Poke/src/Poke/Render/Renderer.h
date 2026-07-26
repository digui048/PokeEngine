#ifndef RENDERER_H
#define RENDERER_H

#include <memory>

namespace Poke
{
    class VertexArray;
    class Shader;
    class VulkanContext;

    class Window;

    class Renderer
    {
    public:
        static void Init(Window& window);
        static void Shutdown();
        static void WaitIdle();

        static void BeginFrame();
        static void EndFrame();
        static void DrawTriangle();

        static void SetClearColor(const float r, const float g, const float b, const float a);
    private:
        static std::unique_ptr<VulkanContext> s_Context;
        static struct ClearColor { float r, g, b, a; } s_ClearColor;
    };
}

#endif