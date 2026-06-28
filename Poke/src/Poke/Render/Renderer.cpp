#include "Renderer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"

#include "Poke/Core/Assert.h"

#include <glad/glad.h>
#include <SDL3/SDL.h>

using namespace Poke;

void Renderer::Init()
{
    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
}

void Renderer::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Poke::Renderer::DrawIndexed(const VertexArray &vertexArray)
{
    vertexArray.Bind();

    IndexBuffer *ebo = vertexArray.GetIndexBuffer();

    POKE_ASSERT(ebo != nullptr, "VertexArray has no IndexBuffer");

    glDrawElements(GL_TRIANGLES, ebo->GetCount(), GL_UNSIGNED_INT, nullptr);
}
