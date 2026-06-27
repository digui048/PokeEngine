#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

namespace Poke
{
    class VertexBuffer;
    class IndexBuffer;

    class VertexArray
    {
    public:
        VertexArray();
        ~VertexArray();

        void AddBuffer(VertexBuffer *vbo, IndexBuffer *ebo);

        void Bind() const;
        void Unbind() const;

    private:
        unsigned int m_rendererID;
        VertexBuffer *m_vbo;
        IndexBuffer *m_ebo;
    };
}

#endif