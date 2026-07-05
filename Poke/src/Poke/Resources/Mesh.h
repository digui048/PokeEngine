#ifndef MESH_H
#define MESH_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>

namespace Poke
{
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    class VertexArray;
    class VertexBuffer;
    class IndexBuffer;

    class Mesh
    {
    public:
        Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);
        ~Mesh();

        Mesh(const Mesh &) = delete;
        Mesh &operator=(const Mesh &) = delete;
        Mesh(Mesh &&other) noexcept;
        Mesh &operator=(Mesh &&other) noexcept;

        void Bind() const;
        void Unbind() const;

        unsigned int GetIndexCount() const { return m_IndexCount; }
        const VertexArray &GetVertexArray() const { return *m_VAO; }

    private:
        std::unique_ptr<VertexArray> m_VAO;
        std::shared_ptr<VertexBuffer> m_VBO;
        std::shared_ptr<IndexBuffer> m_EBO;

        unsigned int m_IndexCount;
    };
}

#endif