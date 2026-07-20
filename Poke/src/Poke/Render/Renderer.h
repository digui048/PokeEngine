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
        static void Clear();
        static void SetClearColor(const float r, const float g, const float b, const float a);
        static void SetViewport(const int x1, const int x2, const int width, const int height);
        static void DrawIndexed(const VertexArray &vertexArray, const Shader &shader);

    private:
        static std::unique_ptr<VulkanContext> s_Context;
    };
}

#endif