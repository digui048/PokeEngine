#ifndef RENDERER_H
#define RENDERER_H

namespace Poke
{
    class VertexArray;

    class Renderer
    {
    public:
        static void Init();
        static void Clear();
        static void DrawIndexed(const VertexArray& vertexArray);
    };
}

#endif