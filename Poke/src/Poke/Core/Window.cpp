#include "Window.h"
#include "Assert.h"

#include <SDL3_image/SDL_image.h>

using namespace Poke;

Window::Window(std::string name, int width, int height)
{
    POKE_ASSERT(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0, "Failed to initialise SDL: {}", SDL_GetError());
    POKE_CORE_INFO("SDL initialized successfully");

    m_window = SDL_CreateWindow(name.c_str(), width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    POKE_ASSERT(m_window != nullptr, "SDL_CreateWindow failed: {}", SDL_GetError());
    POKE_CORE_INFO("Window created: {}x{}", width, height);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    m_GLContext = SDL_GL_CreateContext(m_window);
    POKE_ASSERT(m_GLContext != nullptr, "SDL_GL_CreateContext failed: {}", SDL_GetError());
    POKE_CORE_INFO("OpenGL context created successfully");
    POKE_CORE_TRACE("OpenGL context attributes set (4.6 Core)");
}

Window::~Window()
{
    if (m_window != nullptr)
    {
        SDL_GL_DestroyContext(m_GLContext);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        m_window = nullptr;
    }
    POKE_CORE_INFO("Destroying Window");
}

void Window::GetWindowSize(int &w, int &h)
{
    int width, height;
    SDL_GetWindowSize(m_window, &width, &height);

    w = width;
    h = height;
}

void Window::SetTitle(const std::string &name)
{
    std::string res = "PokeEngine" + name;
    SDL_SetWindowTitle(m_window, res.c_str());
}

bool Window::SetIcon(const std::string &path)
{
    if (!m_window)
    {
        return false;
    }

    SDL_Surface *icon = IMG_Load(path.c_str());
    if (!icon)
    {
        return false;
    }

    SDL_SetWindowIcon(m_window, icon);
    SDL_DestroySurface(icon);
    return true;
}

void Window::SwapWindow() const
{
    SDL_GL_SwapWindow(m_window);
}
