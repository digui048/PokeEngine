#include "Window.h"
#include "Assert.h"
#include "Poke/Render/Renderer.h"

#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_vulkan.h>

using namespace Poke;

Window::Window(std::string name, int width, int height)
{
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11, wayland");

    POKE_ASSERT(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0, "Failed to initialise SDL: {}", SDL_GetError());
    POKE_CORE_INFO("SDL initialized successfully");

    POKE_CORE_INFO("SDL is using video driver: {0}", SDL_GetCurrentVideoDriver());

    m_window = SDL_CreateWindow(name.c_str(), width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    POKE_ASSERT(m_window != nullptr, "SDL_CreateWindow failed: {}", SDL_GetError());
    POKE_CORE_INFO("Window created: {}x{}", width, height);
}

Window::~Window()
{
    if (m_window != nullptr)
    {
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
}
