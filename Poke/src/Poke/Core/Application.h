#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

namespace Poke
{
    class Window;
    class ImGuiManager;

    class Application
    {
    public:
        Application();
        virtual ~Application();

        static Application &GetInstance();

        void Run();

        Window *GetWindow() const { return m_window.get(); }

    protected:
        virtual void OnInit();
        virtual void OnUpdate(float dt);
        virtual void OnShutdown();

    private:
        void PollEvents();

    private:
        static Application *s_Instance;
        std::unique_ptr<Window> m_window;
        std::unique_ptr<ImGuiManager> m_imguiManager;
        bool m_Running = true;
    };
}

#endif