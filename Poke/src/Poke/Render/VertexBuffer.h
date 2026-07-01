#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include "VertexBufferLayout.h"

namespace Poke
{
    class VertexBuffer
    {
    public:
        VertexBuffer(const void *data, unsigned int size);
        ~VertexBuffer();

        void Bind() const;
        void Unbind() const;

        void SetLayout(const VertexBufferLayout& layout) { m_layout = layout; }
        const VertexBufferLayout& GetLayout() const { return m_layout; }

    private:
        unsigned int m_rendererID;
        VertexBufferLayout m_layout;
    };
}

#endif