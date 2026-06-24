#ifndef WINDOW_H
#define WINDOW_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>

#include <string>

#define WINDOW_PREV_WIDTH 1200
#define WINDOW_PREV_HEIGHT 800

namespace Poke
{
    class Window
    {
    public:
        Window(std::string name, int width, int height);
        ~Window();

        SDL_Window *GetSDLWindow() const { return m_window; }
        SDL_GLContext GetSDLContext() const { return m_GLContext; }
        void GetWindowSize(int &w, int &h);

        void SetTitle(const std::string &name);
        bool SetIcon(const std::string &path);

        void SwapWindow() const;

    private:
        SDL_Window *m_window = nullptr;
        SDL_GLContext m_GLContext = nullptr;
    };
}

#endif