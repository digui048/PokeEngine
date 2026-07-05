#include "Mesh.h"

#include "Poke/Render/VertexArray.h"
#include "Poke/Render/VertexBuffer.h"
#include "Poke/Render/IndexBuffer.h"

using namespace Poke;

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices)
    : m_IndexCount(static_cast<unsigned int>(indices.size()))
{
    m_VAO = std::make_unique<VertexArray>();
    m_VBO = std::make_shared<VertexBuffer>((void *)vertices.data(), static_cast<unsigned int>(vertices.size() * sizeof(Vertex)));

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);
    layout.Push<float>(2);
    m_VBO->SetLayout(layout);

    m_EBO = std::make_shared<IndexBuffer>(indices.data(), m_IndexCount);

    m_VAO->Bind();
    m_VBO->Bind();
    m_EBO->Bind();
    m_VAO->AddBuffer(m_VBO.get(), m_EBO.get());

    m_VAO->Unbind();
    m_VBO->Unbind();
    m_EBO->Unbind();
}

Mesh::~Mesh() = default;

Mesh::Mesh(Mesh &&other) noexcept = default;
Mesh& Mesh::operator=(Mesh &&other) noexcept = default;

void Mesh::Bind() const
{
    m_VAO->Bind();
}

void Mesh::Unbind() const
{
    m_VAO->Unbind();
}
