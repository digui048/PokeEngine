#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <vector>

namespace Poke
{
    class Window;
    class ImGuiManager;
    class Module;

    class Application
    {
    public:
        Application();
        virtual ~Application();

        static Application &GetInstance();

        void Run();

        void PushModule(std::shared_ptr<Module> module);
        void ClearModules();

        Window *GetWindow() const { return m_window.get(); }

        void ForceQuit();

        void SwitchModule(std::shared_ptr<Module> newModule);

    protected:
        virtual void OnInit() {}
        virtual void OnUpdate(float dt) {}
        virtual void OnShutdown() {}

    private:
        void PollEvents();

    private:
        static Application *s_Instance;
        std::unique_ptr<Window> m_window;
        std::unique_ptr<ImGuiManager> m_imguiManager;
        bool m_Running = true;

        std::vector<std::shared_ptr<Module>> m_modules;
        std::shared_ptr<Module> m_pendingModule;
    };
}

#endif