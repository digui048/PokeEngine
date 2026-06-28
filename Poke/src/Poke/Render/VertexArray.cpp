#include "VertexArray.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"

#include <glad/glad.h>

using namespace Poke;

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_rendererID);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_rendererID);
}

void VertexArray::AddBuffer(VertexBuffer *vbo, IndexBuffer *ebo)
{
    m_vbo = vbo;
    m_ebo = ebo;

    glBindVertexArray(m_rendererID);
    vbo->Bind();
    ebo->Bind();
    const auto &elements = vbo->GetLayout().GetElements();
    size_t offset = 0;
    for (size_t i = 0; i < elements.size(); ++i)
    {
        const auto &element = elements[i];
        glVertexAttribPointer(i, element.m_count, element.m_type, element.m_normalized, vbo->GetLayout().GetStride(), (const void *)offset);
        glEnableVertexAttribArray(i);
        offset += element.m_count * VertexBufferElement::GetSize(element.m_type);
    }
}

void VertexArray::Bind() const
{
    glBindVertexArray(m_rendererID);
}

void VertexArray::Unbind() const
{
    glBindVertexArray(0);
}
