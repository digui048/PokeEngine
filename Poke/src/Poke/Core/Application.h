#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <vector>

namespace Poke
{
    class Window;
    class ImGuiManager;
    class Module;
    class Mesh;
    class UniformBuffer;
    class VulkanPipeline;
    class VulkanTexture;

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

        std::unique_ptr<Mesh> m_mesh;

        std::unique_ptr<UniformBuffer> m_uniformBuffer;

        std::shared_ptr<VulkanPipeline> m_defaultPipeline;
        std::shared_ptr<VulkanTexture> m_texture;
    };
}

#endif