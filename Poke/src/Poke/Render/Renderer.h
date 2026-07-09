#ifndef RENDERER_H
#define RENDERER_H

namespace Poke
{
    class VertexArray;
    class Shader;
    class VulkanContext;

    class Renderer
    {
    public:
        static void Init();
        static void Clear();
        static void SetClearColor(const float r, const float g, const float b, const float a);
        static void SetViewport(const int x1, const int x2, const int width, const int height);
        static void DrawIndexed(const VertexArray &vertexArray, const Shader &shader);

    private:
        static VulkanContext s_Context;
    };
}

#endif