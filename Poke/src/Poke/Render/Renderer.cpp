#include "Renderer.h"

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