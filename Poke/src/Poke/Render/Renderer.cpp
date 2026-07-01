#include "Renderer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

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

void Renderer::SetClearColor(const float r, const float g, const float b, const float a)
{
    glClearColor(r, g, b, a);
}

void Renderer::SetViewport(const int x1, const int x2, const int width, const int height)
{
    glViewport(x1, x2, width, height);
}

void Poke::Renderer::DrawIndexed(const VertexArray &vertexArray, const Shader &shader)
{
    vertexArray.Bind();
    shader.Bind();

    glDrawElements(GL_TRIANGLES, vertexArray.GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
}
