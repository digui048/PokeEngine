#include "Window.h"

#include <SDL3_image/SDL_image.h>

using namespace Poke;

Window::Window(std::string name, int width, int height)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    m_window = SDL_CreateWindow(name.c_str(), width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    m_GLContext = SDL_GL_CreateContext(m_window);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
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
