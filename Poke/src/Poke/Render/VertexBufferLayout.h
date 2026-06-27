#ifndef VERTEX_BUFFER_LAYOUT_H
#define VERTEX_BUFFER_LAYOUT_H

#include <glad/glad.h>
#include <vector>

namespace Poke
{
    struct VertexBufferElement
    {
        unsigned int m_type;
        unsigned int m_count;
        unsigned char m_normalized;

        static unsigned int GetSize(unsigned int type)
        {
            switch (type)
            {
            case GL_FLOAT:
                return 4;
            case GL_UNSIGNED_INT:
                return 4;
            case GL_UNSIGNED_BYTE:
                return 1;
            }
            return 0;
        }
    };

    class VertexBufferLayout
    {
    private:
        std::vector<VertexBufferElement> elements;
        unsigned int stride;

    public:
        VertexBufferLayout() : stride(0) {}

        template <typename T>
        void Push(unsigned int count) = delete;

        inline const std::vector<VertexBufferElement> &GetElements() const { return elements; }
        inline unsigned int GetStride() const { return stride; }
    };

    template <>
    void VertexBufferLayout::Push<float>(unsigned int count)
    {
        VertexBufferElement element = {GL_FLOAT, count, GL_FALSE};
        elements.push_back(element);
        stride += count * VertexBufferElement::GetSize(GL_FLOAT);
    }

    template <>
    void VertexBufferLayout::Push<unsigned int>(unsigned int count)
    {
        VertexBufferElement element = {GL_UNSIGNED_INT, count, GL_FALSE};
        elements.push_back(element);
        stride += count * VertexBufferElement::GetSize(GL_UNSIGNED_INT);
    }

    template <>
    void VertexBufferLayout::Push<unsigned char>(unsigned int count)
    {
        VertexBufferElement element = {GL_UNSIGNED_BYTE, count, GL_TRUE};
        elements.push_back(element);
        stride += count * VertexBufferElement::GetSize(GL_UNSIGNED_BYTE);
    }
}

#endif